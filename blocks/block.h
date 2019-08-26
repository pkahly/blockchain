#include <gmpxx.h>

struct Metadata {
   int height;
   mpz_class hash;
};

struct Header {
   mpz_class previous_blockhash = -1;
   mpz_class nonce = 0;
   mpz_class difficulty_target = 1.0e+136;
   time_t timestamp = time(NULL);
};

class Block {
   public: // TODO make private
      Metadata metadata;
      Header header;
};
