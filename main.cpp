#include "blocks/block.h"
#include "blocks/blockchain.h"
#include "mining/mining.h"

#include <iostream>
using namespace std;

/** Start mining */
int main() {
   // Create blockchain
   Blockchain blockchain;
   
   // Create genesis block
   Block genesis_block = create_new_genesis_block();
   blockchain.add_block(0, genesis_block);
   
   // Mine new blocks
   Block block = genesis_block;
   int height = 1;
   
   while (true) {
      block = mine_new_block(blockchain, height);
      
      blockchain.add_block(height, block);
      
      cout << "\n";
      cout << "height: " << height << "\n";
      cout << "hash: " << block.metadata.hash << "\n";
      cout << "difficulty: " << block.header.difficulty_target << "\n";
      cout << "timestamp: " << block.header.timestamp << "\n";
      
      height++;
   }
}
