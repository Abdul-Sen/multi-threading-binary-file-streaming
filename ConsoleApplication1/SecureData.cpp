 // Workshop 10 - Multi-Threading
 // SecureData.cpp

 #include <iostream>
 #include <fstream>
 #include <string>
 #include <thread>
 #include <functional>
 #include "SecureData.h"

 namespace w10 {

     void converter(char* t, char key, int n, const Cryptor& c) {
         for (int i = 0; i < n; i++)
             t[i] = c(t[i], key);
     }

     SecureData::SecureData(const char* file, char key, std::ofstream* pOfs) {

		 ofs = pOfs;

		 // open text file
         std::fstream input(file, std::ios::in);
         if (!input)
             throw std::string("\n***Failed to open file ") +
              std::string(file) + std::string(" ***\n");

         // copy from file into memory
         nbytes = 0;
         input >> std::noskipws;
         while (input.good()) {
             char c;
             input >> c;
             nbytes++;
         }
         nbytes--;
         input.clear();
         input.seekg(0, std::ios::beg);
         text  = new char[nbytes + 1];

         int i = 0;
         while (input.good())
             input >> text[i++];
         text[--i] = '\0';
         *ofs << "\n" << nbytes << " bytes copied from text "
          << file << " into memory (null byte added)\n";
         encoded = false;

         // encode using key
         code(key);
         *ofs << "Data encrypted in memory\n";
     }

     SecureData::~SecureData() {
         delete [] text;
     }

     void SecureData::display(std::ostream& os) const {
         if (text && !encoded)
             os << text << std::endl;
         else if (encoded)
             throw std::string("\n***Data is encoded***\n");
         else
             throw std::string("\n***No data stored***\n");
     }

     void SecureData::code(char key) {
      //   converter(text, key, nbytes, Cryptor());
		 std::thread t1(std::bind(converter, text, key, nbytes / 2, Cryptor()));

		 std::thread t2(std::bind(converter, text + nbytes/2, key, nbytes - nbytes / 2, Cryptor()));

		 t1.join();
		 t2.join();
         encoded = !encoded;
     }

     void SecureData::backup(const char* file) {
         if (!text)
             throw std::string("\n***No data stored***\n");
         else if (!encoded)
             throw std::string("\n***Data is not encoded***\n");
         else {

    		// open binary file
			 std::ofstream f(file, std::ios::out|std::ios::binary);
			 if (!f.is_open())
				 throw "You need to learn how to open files";
	        // write binary file here
			 f.write(text, this->nbytes);
			 f.close();
         }
     }

     void SecureData::restore(const char* file, char key) {
         // open binary file
		 std::ifstream fin(file, std::ios::in | std::ios::binary);
		 if (!fin.is_open())
			 throw "Binary File not open";

         // allocate memory here
		 delete[] text;
		 text = new char[nbytes];

         // read binary file here
		 fin.read(text, nbytes);
		 fin.close();

         *ofs << "\n" << nbytes + 1 << " bytes copied from binary file "
          << file << " into memory (null byte included)\n";

         // decode using key
         code(key);
         *ofs << "Data decrypted in memory\n\n";
		 text[nbytes - 1] = '\0';

     }

     std::ostream& operator<<(std::ostream& os, const SecureData& sd) {
         sd.display(os);
         return os;
     }

 }
