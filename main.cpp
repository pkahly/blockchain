#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>
#include <openssl/rand.h>
#include <openssl/sha.h>
using namespace std;

struct Metadata {
   int height;
   string hash;
};

struct Header {
   string previous_blockhash;
   int num_zeros;
   string nonce;
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
string get_random_nonce() {
   unsigned char* nonce = new unsigned char[SHA512_DIGEST_LENGTH];
   RAND_bytes(nonce, SHA512_DIGEST_LENGTH);
   return to_hex(nonce);
}

/**
* Return the SHA512 hash of the given string
*/
string sha512(string line) {    
   unsigned char hash[SHA512_DIGEST_LENGTH];
   SHA512_CTX sha512;
   SHA512_Init(&sha512);
   SHA512_Update(&sha512, line.c_str(), line.length());
   SHA512_Final(hash, &sha512);

   return to_hex(hash);
}

/**
* Combine the block header fields and hash them
*/
string get_blockhash(Header header) {
   string header_str = header.previous_blockhash 
      + (char)header.num_zeros
      + header.nonce;

   return sha512(header_str);
}

/**
* Create a new genesis block to build the blockchain on
*/
Block create_new_genesis_block() {
   Block genesis_block;
   
   genesis_block.header.previous_blockhash = "None";
   genesis_block.header.num_zeros = 0;
   genesis_block.header.nonce = get_random_nonce();
   
   genesis_block.metadata.height = 0;
   genesis_block.metadata.hash = get_blockhash(genesis_block.header);
   
   return genesis_block;
}

/**
* Verifies that the hash has the right number of zeros at the beginning
*/
bool valid_hash(string hash, int num_zeros) {
   for (int i = 0; i < num_zeros; i++) {
      if (hash.at(i) != '0') {
         return false;
      }
   }
   
   return true;
}

/**
* Mine new block based on the given block
*/
Block mine_new_block(Block previous_block) {
   Block block;
   
   block.header.previous_blockhash = previous_block.metadata.hash;
   block.metadata.height = previous_block.metadata.height + 1;
   
   block.header.num_zeros = 2;
   
   do {
      block.header.nonce = get_random_nonce();
      
      block.metadata.hash = get_blockhash(block.header);
   } while (!valid_hash(block.metadata.hash, block.header.num_zeros));
   
   return block;
}

/** Start mining */
int main() {
   Block genesis_block = create_new_genesis_block();
   cout << genesis_block.metadata.hash << "\n";
   
   Block block = genesis_block;
   while (true) {
      block = mine_new_block(block);
      cout << block.metadata.hash << "\n";
   }
}
