#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include <hpx/lcos/gather.hpp>
#include <hpx/lcos/local/event.hpp>
#include <hpx/lcos/local/dataflow.hpp>
#include <boost/serialization/vector.hpp>
#include "block.hpp"

char const* gather_basename = "/main/gather/";

using namespace std;

int main(int argc, char* argv[])
{
    using namespace boost::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()

    	("bs", value<boost::uint64_t>()->default_value(64), 
	"approximate Size (in each dimension) of Blocks (will be adjusted to fit to grid)")

	("ds", value<boost::uint64_t>()->default_value(64), 
        "Number of grid points (in each dimension)")

	("path", value<std::string>()->default_value("file.dat"), 
        "Path to File with scalar field data")

	("iso", value<boost::uint64_t>()->default_value(0.2), "isovalue to use") 
        ( "no-header", "do not print out the csv header row")
    ;

    // Initialize and run HPX. Run hpx_main on all Localities
    std::vector<std::string> cfg;
    cfg.push_back("hpx.run_hpx_main!=1");

    return hpx::init(desc_commandline, argc, argv, cfg);
}

boost::uint64_t calculateBlockNum(boost::uint64_t& dataSize, boost::uint64_t& blockSize){
	boost::uint64_t fitnum = (boost::uint64_t)dataSize/blockSize;
	blockSize = (boost::uint64_t)dataSize/fitnum;
	dataSize = blockSize*fitnum;
	return fitnum;
}

glm::vec3 calculateOffset(boost::uint64_t locID, boost::uint64_t blockNum, boost::uint64_t blockPerLoc){
	glm::vec3 result;
	boost::uint64_t blockOffset = blockPerLoc*locID;
	result.x = (boost::uint64_t)blockOffset/(blockNum*blockNum);
	result.z = (boost::uint64_t)(blockOffset % (blockNum*blockNum))/blockNum;
	result.y = (boost::uint64_t)blockOffset % blockNum;
	return result;				
}

std::vector<block> generateBlocks(boost::uint64_t num, glm::vec3 pos, boost::uint64_t blockNum, boost::uint64_t blockSize){	
	std::vector<block> result;		
	for (boost::uint64_t i = 0; (i < num); i++){
		serialVec position;
		position.x = pos.x;
		position.y = pos.y;
		position.z = pos.z;
		block* instance = new block(
			hpx::components::new_<server::block>(hpx::find_here(), position, blockSize) );
		instance->initialize();
		result.push_back(*instance);
		delete instance;
		pos.y++;
		if (!( ((int)pos.y) % blockNum)){
			pos.y = 0;
			pos.z++;
			if (!(((int)pos.z) % blockNum)){
				pos.z = 0;
				pos.x++;
			}
		}
	} 
	return result;	
}

namespace broadcastCatching {
	hpx::lcos::local::event block_boilerplatesReady;
	std::vector<block_boilerplate> block_boilerplates;

	void catchBroadcast(std::vector<block_boilerplate> globalBlocks){
		block_boilerplates = globalBlocks;
		block_boilerplatesReady.set();
	}
	
	std::vector<block_boilerplate> waitBroadcast(){
		block_boilerplatesReady.wait();
		return block_boilerplates;
	}
}

HPX_PLAIN_ACTION(broadcastCatching::catchBroadcast, a_catchBroadcast);
HPX_REGISTER_GATHER(std::vector<block_boilerplate>, id_type_gatherer);

int hpx_main(boost::program_options::variables_map& vm)
{
	printf("+++++++++++++++++++++HPX Inizialized\n");
	//read command-line parameters
 	boost::uint64_t blockSize = vm["bs"].as<boost::uint64_t>();
	boost::uint64_t dataSize = vm["ds"].as<boost::uint64_t>();
	boost::uint64_t isovalue = vm["iso"].as<boost::uint64_t>();
	std::string path = vm["path"].as<std::string>();

	printf("Found parameters: blocksize: %u, datasize: %u \n", blockSize, dataSize);

	//find number of localities 
	std::vector<hpx::id_type> localities = hpx::find_all_localities();
    	boost::uint64_t nl = localities.size(); 

	//calculate number of blocks needed for data size (# of blocks for each dimension)
	boost::uint64_t blockNum = calculateBlockNum(dataSize, blockSize);

	//calculate number of blocks per locality (except last one)
	boost::uint64_t blockCount = blockNum*blockNum*blockNum;
	boost::uint64_t blockPerLoc = (boost::uint64_t)blockCount/nl;

	//calculate block space offset of this localities blocks 
	glm::vec3 offset = calculateOffset(hpx::get_locality_id(), blockNum, blockPerLoc);

	//create blocks (scheduling data loading)
	std::vector<block> block_handlers;
	if (hpx::get_locality_id() < (nl-1)){
		block_handlers = generateBlocks(blockPerLoc, offset, blockNum, blockSize);
	} else {
		block_handlers = generateBlocks(
					blockCount-(blockPerLoc*(nl-1)), 
					offset, 
					blockNum, blockSize);
	}

	printf("+++++++++++++++++++++Blocks Created\n");

	//extract block hpx_ids and block space positions (block boilerplate)
	std::vector<block_boilerplate> myboilerplates;
	for (block block : block_handlers){
		myboilerplates.push_back(block.getBoilerplate());
	}
	
	printf("+++++++++++++++++++++Boilerplates recieved\n");

	//package boilerplates in future for gathering
 	hpx::future<std::vector<block_boilerplate>> f_boilerplates = 
		hpx::make_ready_future(myboilerplates);	
	
	hpx::shared_future<std::vector<block_boilerplate>> block_boilerplates;

	//schedule gathering, merging and broadcasting of boilerplates
	if (0 == hpx::get_locality_id())
    	{
	    //gather boilerplates for all blocks at locality 0 
	    //(scheduled when boilerplates ready)
	    hpx::future<std::vector<std::vector<block_boilerplate>>> every_boilerplates =
            	hpx::lcos::gather_here(gather_basename, std::move(f_boilerplates), nl);
	    
	    //merge all locality-wide boilerplate lists and broadcast back to each locality
	    //(scheduled when gathered lists ready)
	    block_boilerplates = every_boilerplates.then(
        	[&](hpx::future<std::vector<std::vector<block_boilerplate>>> boilerplateslists)
		-> std::vector<block_boilerplate>
        	{
		    std::vector<block_boilerplate> result;
            	    for (std::vector<block_boilerplate> currentVec : boilerplateslists.get()){
			for (block_boilerplate currentInfo : currentVec){
    			    result.push_back(currentInfo);
			}
		    }
		
		    for (hpx::id_type loc : hpx::find_remote_localities() ){
			hpx::apply<a_catchBroadcast>(loc, result);
	            }
	
		    printf("+++++++++++++++++++++Boilerplate gathered, broadcast scheduled\n");
		
	            return result;
        	}); 
	} else {
	    //gather boilerplates for this localities blocks at locality 0
            hpx::lcos::gather_there(gather_basename, std::move(f_boilerplates));
	    block_boilerplates = hpx::async(broadcastCatching::waitBroadcast);
	}

	printf("+++++++++++++++++++++Boilerplate gathering scheduled\n");

	for (block currentBlock : block_handlers) {

		//Schedule marching cubes first phase (finding vertex-edges and faces)
		hpx::shared_future<int> blockInfoFound = currentBlock.findInfo(isovalue);
	
		printf("+++++++++++++++++++++find Info scheduled\n");
		
		//Schedule marching cubes second phase (vertex positions and normals)
	        //when first Phase ready
		hpx::shared_future<int> blockResultFound = blockInfoFound.then(
				[=](hpx::shared_future<int> f) -> int {
					printf("+++++++++++++++++++++Found Info, start compute Block\n");
					return currentBlock.computeBlock(isovalue).get();
				});

		printf("+++++++++++++++++++++compute Block Scheduled\n");

		//Schedule communication of vertex unique id offset 
	        //when first phase and global block info ready
		hpx::shared_future<int> globalIDsFound = hpx::lcos::local::dataflow(
				[=](hpx::shared_future<int> blockInfoFound, hpx::shared_future<std::vector<block_boilerplate>> block_boilerplates) -> int 
				{
					printf("+++++++++++++++++++++Found Info, start communicate global IDs\n");
					return currentBlock.communicateGlobalIDs(block_boilerplates.get()).get();
				}, std::move(blockInfoFound), block_boilerplates); 

		printf("+++++++++++++++++++++communicate Global IDs scheduled\n");

		//Schedule sending of normals to neighbor-blocks when second Phase and global block info ready
		hpx::lcos::local::dataflow(
				[=](hpx::shared_future<int> blockResultFound, hpx::shared_future<std::vector<block_boilerplate>> block_boilerplates) 
				{
					printf("+++++++++++++++++++++computed Block, start send passive\n");
					currentBlock.sendPassive(block_boilerplates.get());
				}, std::move(blockResultFound), block_boilerplates); 

		printf("+++++++++++++++++++++send Passive Scheduled\n");

		//Schedule sending of unique ids to neighbor-blocks when unique ids ready
		hpx::lcos::local::dataflow(
				[=](hpx::shared_future<int> globalIDsFound, hpx::shared_future<std::vector<block_boilerplate>> block_boilerplates)
				{
					printf("+++++++++++++++++++++ found global IDs, start send active\n");
					currentBlock.sendActive(block_boilerplates.get());
				}, std::move(globalIDsFound), block_boilerplates); 
		printf("+++++++++++++++++++++send active Scheduled\n");
	}
	//Wait for all blocks to finish with semaphore
	return hpx::finalize();
}


