#include <gmpxx.h>

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

class Block {
   public: // TODO make private and add accessors
      Metadata metadata;
      Header header;
};
