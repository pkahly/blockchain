#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>
#include <map>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <gmpxx.h>
using namespace std;

struct Metadata {
   int height;
   mpz_class hash;
};

struct Header {
   mpz_class previous_blockhash;
   mpz_class nonce;
   mpz_class difficulty_target;
   time_t timestamp = time(NULL);
};

struct Block {
   Metadata metadata;
   Header header;
};

/**
* Convert an unsigned char to a string of hex characters
*/
string to_hex(unsigned char* s) {
   stringstream ss;
   
   for(int i = 0; i < SHA512_DIGEST_LENGTH; i++) {
      ss << hex << (int) s[i];
   }
   
   return ss.str();
}   

/**
* Generate a Random Nonce using SSL's cryptographically secure random
*/
mpz_class get_random_nonce() {
   unsigned char* nonce = new unsigned char[SHA512_DIGEST_LENGTH];
   RAND_bytes(nonce, SHA512_DIGEST_LENGTH);
   
   string nonce_str = to_hex(nonce);
   
   mpz_class nonce_mpz;
   nonce_mpz.set_str(nonce_str, 16);   
   
   return nonce_mpz;
}

/**
* Return the SHA512 hash of the given string
*/
mpz_class sha512(string line) {    
   unsigned char hash[SHA512_DIGEST_LENGTH];
   SHA512_CTX sha512;
   SHA512_Init(&sha512);
   SHA512_Update(&sha512, line.c_str(), line.length());
   SHA512_Final(hash, &sha512);

   string hash_str = to_hex(hash);
   
   mpz_class hash_mpz;
   hash_mpz.set_str(hash_str, 16);   
   
   return hash_mpz;
}

/**
* Combine the block header fields and hash them
*/
mpz_class get_blockhash(Header header) {
   stringstream ss;
   
   ss << header.previous_blockhash << header.difficulty_target << header.nonce << header.timestamp;

   return sha512(ss.str());
}

/**
* Calculate the next difficulty target based on the timestamps of the previous blocks
*/
mpz_class get_next_difficulty(map<int, Block> blockchain, int height) {
   // Settings
   int difficulty_period = 100;
   int idea_block_time_seconds = 10;
   float max_ratio = 4;
   float min_ratio = 0.25;
   
   // Get block heights to use
   int tip_height = height - 1;
   int old_height = tip_height - difficulty_period;
   if (old_height < 0) {
      old_height = 0;
   }
   int count = tip_height - old_height;
   
   // Get average time between blocks
   Block tip_block = blockchain[tip_height];
   Block old_block = blockchain[old_height];
   int total_seconds = tip_block.header.timestamp - old_block.header.timestamp;
   
   int average_seconds;
   if (count > 0) {
      average_seconds = total_seconds / count;
   } else {
      average_seconds = 0;
   }
   
   // Calculate ratio
   float ratio;
   if (average_seconds > 0) {
      ratio = float(idea_block_time_seconds) / average_seconds;
   } else {
      ratio = max_ratio;
   }
   
   if (ratio < min_ratio) {
      ratio = min_ratio;
   } else if (ratio > max_ratio) {
      ratio = max_ratio;
   }
   
   cout << "\n" << "Difficulty Ratio: " << ratio << "\n";
   
   // Calculate new difficulty
   mpz_class prev_diff = tip_block.header.difficulty_target;
   
   mpz_class new_diff;
   if (ratio > 1) {
      new_diff = prev_diff / ratio;
   } else {
      // Apparently, mpz_class doesn't support dividing by numbers less than 1
      new_diff = prev_diff * (1 / ratio);
   }
   
   return new_diff;
}

/**
* Create a new genesis block to build the blockchain on
*/
Block create_new_genesis_block() {
   Block genesis_block;
   
   genesis_block.header.previous_blockhash = -1;
   genesis_block.header.difficulty_target =  1.340781e+154;
   genesis_block.header.nonce = get_random_nonce();
   
   genesis_block.metadata.height = 0;
   genesis_block.metadata.hash = get_blockhash(genesis_block.header);
   
   return genesis_block;
}

/**
* Mine new block based on the given block
*/
Block mine_new_block(map<int, Block> blockchain, int height) {
   Block previous_block = blockchain[height - 1];
   Block block;
   
   block.header.previous_blockhash = previous_block.metadata.hash;
   block.metadata.height = previous_block.metadata.height + 1;
   
   block.header.difficulty_target = get_next_difficulty(blockchain, height);
   
   do {
      block.header.nonce = get_random_nonce();
      block.header.timestamp = time(NULL);
      
      block.metadata.hash = get_blockhash(block.header);
   } while (block.metadata.hash > block.header.difficulty_target);
   
   return block;
}

/** Start mining */
int main() {
   // Create blockchain. height -> block
   map<int, Block> blockchain;
   
   // Create genesis block
   Block genesis_block = create_new_genesis_block();
   blockchain[0] = genesis_block;
   
   // Mine new blocks
   Block block = genesis_block;
   int height = 1;
   
   while (true) {
      block = mine_new_block(blockchain, height);
      blockchain[height] = block;
      
      cout << "\n";
      cout << "height: " << height << "\n";
      cout << "hash: " << block.metadata.hash << "\n";
      cout << "difficulty: " << block.header.difficulty_target << "\n";
      cout << "timestamp: " << block.header.timestamp << "\n";
      
      height++;
   }
}
