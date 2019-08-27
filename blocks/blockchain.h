#include <map>
using namespace std;

class Blockchain {
   private:
      map<int, Block> height_to_block;
      
   public:
      void add_block(int height, Block block) {
         height_to_block[height] = block;
      }
      
      Block get_block(int height) {
         return height_to_block[height];
      }
};
