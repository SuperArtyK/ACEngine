#ifndef _AEFILEWRITER_HPP
#define _AEFILEWRITER_HPP

#include <cstring>
#include <string>
#include <vector>
#include <filesystem>




//File flags

///Write cursor at the end of the file, adding to the file
#define AEFW_FLAG_APPEND (unsigned char)1
///Write cursor at the end of the file, adding to the file
///Changing the cursor position(fseek)/writing anywhere else is not allowed
#define AEFW_FLAG_APPEND_NO_CURSOR_MOVE (unsigned char)2
///Write cursor at the start of the file, truncating the contents if existed
#define AEFW_FLAG_TRUNCATE (unsigned char)3
///No Write flags
#define AEFW_FLAG_NOFLAGS (unsigned char)255

//Error flags

///indicator that everything is good
#define AEFW_ERR_NOERROR 0


//file creation/manipulation
///If the file name is empty
#define AEFW_ERR_FILE_NAME_EMPTY 1
///If the flag given is wrong during the creation(creates file anyway, assuming AEFW_FLAG_NOFLAGS)
#define AEFW_ERR_FILE_WRONG_FLAG 2
///If the file couldn't be created for some other reason, like missing permissions to access files
#define AEFW_ERR_FILE_ELSE 3
///If the file object we're trying to write to is null,
///aka file not open.
///Usually will come after AEFW_ERR_FILE_NAME_EMPTY or AEFW_ERR_FILE_ELSE,
///if we continue to access the writer.
#define AEFW_ERR_WRT_FILE_NULL 11



class AEFileWriter{
public:
	/// <summary>
	/// Class constructor
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening: 0 -- nothing, 1 -- append to end; 2 -- clear file when opening</param>
	/// <param name="af_interval">interval in file writes between automatic file flushing </param>
	AEFileWriter(const std::string& filename = "", const unsigned char flags = AEFW_FLAG_APPEND, const std::size_t af_interval = 1) :
		m_filestr(nullptr), m_writtenEntries(0),
		m_autoflushInterval(af_interval), m_lastError(0) {

		this->openFile(filename, flags);
	}

	~AEFileWriter(){
		flushFile();
		closeFile();
	}
	/// <summary>
	/// Write std::string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeStrToFile(const std::string& str, const bool includeNull = false, const bool useAutoFlush = true){
		this->writeDataToFile_Ptr(str.c_str(), str.size() + ((includeNull)?1:0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write std::vector of char (string), as is, to file, and flush if necessary
	/// </summary>
	/// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeStrToFile(const std::vector<char>& str, const bool includeNull = false, const bool useAutoFlush = true){
		// since vector may actually have NULL-terminating char
		// remove it if includeNull is false
		// decrease str.size() by 1
		this->writeDataToFile_Ptr(str.data(), str.size() - ( ( !includeNull && str[str.size()-1] == '\0' )?1:0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write c-string, as is, to file, and flush if necessary
	/// </summary>
	/// <param name="cdata">С-String to write</param>
	/// <param name="dcount">length of string</param>
	/// <param name="includeNull">Flag to include 1 null-terminating character in the string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	inline void writeStrToFile(const char* cdata, const bool includeNull = false, const bool useAutoFlush = true){
		this->writeDataToFile_Ptr(cdata, strlen(cdata) + ((includeNull) ? 1 : 0), sizeof(char), useAutoFlush);
	}


	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary
	/// Difference with writeDataToFile_Ref(): uses (const) pointer to the variable/data
	/// @note Doesn't work(fine) with literals
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="dcount">number of elements in an object</param>
	/// <param name="dsize">size, in bytes, for each element</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	void writeDataToFile_Ptr(const void* cdata, const std::size_t dcount, const std::size_t dsize = sizeof(char), const bool useAutoFlush = true){
		if(isOpen()){
			fwrite(cdata, dsize, dcount, m_filestr);
			if(useAutoFlush){
				if(!(++m_writtenEntries % m_autoflushInterval)){
					flushFile();
				}
			}

		}
		else{
			m_lastError = AEFW_ERR_WRT_FILE_NULL;
			return;
		}
	}

	/// <summary>
	/// Write binary data, as is, to file, and flush if necessary.
	/// Difference with writeDataToFile_Ptr(): uses (const) reference to the variable.
	/// @note Works with literals
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	template<typename T>
	void writeDataToFile_Ref(const T& cdata, const std::size_t tsize = sizeof(T), const bool useAutoFlush = true){
		writeDataToFile_Ptr(&cdata, 1, tsize, useAutoFlush);
	}

	/// <summary>
	/// Determines type of the data passed and uses according procedure to write it to file
	/// @note character string types(vector char, std::string, char*) do not include null-termination character
	/// Invokes other member functions
	/// </summary>
	/// <param name="cdata">(pointer to) object to write</param>
	/// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
	
	template<typename T>
	inline void writeToFile(const T& var, const bool useAutoFlush = true){
		//char array for formatting
		//classic 4*size of the variable
		char formArr[sizeof(var)*4];
		
		//great line of if's to check the types
		//since we don't have a constexpr version of switch
		//and we don't need it anyway
		
		//checks for types that don't require formatting with sprintf
		//single chars
		if constexpr(std::is_same<T, char>::value || std::is_same<T, unsigned char>::value ||
			//const versions
			std::is_same<T, const char>::value || std::is_same<T, const unsigned char>::value){
			
			//the type is well, char
			writeDataToFile_Ptr(&var, 1, sizeof(char), useAutoFlush);
			return;
		}
		//c-strings
		else if constexpr (std::is_same<T, char*>::value || std::is_same<T, unsigned char*>::value || 
			//const versions
			std::is_same<T, const char*>::value || std::is_same<T, const unsigned char*>::value) {
			//no null-terminating char
			//use writeStrToFile() directly instead
			writeStrToFile((const char*)var, false, useAutoFlush);
			return;
		}
		//std::vector's of char or std::string's
		//no unsigned char because you cannot just
		//cast std::vector of unsigned char to std::vector of char
		else if constexpr (std::is_same<T, std::vector<char>>::value || std::is_same<T, std::string>::value ||
			//const versions
			std::is_same<T, const std::vector<char>>::value || std::is_same<T, const std::string>::value) {
			//no null-terminating char
			//use writeStrToFile() directly instead
			writeStrToFile(var, false, useAutoFlush);
			return;
		}

		//stuff that needs formatting
		// integer types:
		
		//shorts and signed ints and booleans
		else if constexpr (std::is_same<T, short>::value || std::is_same<T, unsigned short>::value || std::is_same<T, int>::value || std::is_same<T, bool>::value ||
			//const versions
			std::is_same<T, const short>::value || std::is_same<T, const unsigned short>::value || std::is_same<T, const int>::value || std::is_same<T, const bool>::value) {
			
			sprintf(formArr, "%d", (int)var);
		}
		//unsigned ints
		else if constexpr (std::is_same<T, unsigned int>::value || 
			//const versions
			std::is_same<T, const unsigned int>::value ) {
			
			sprintf(formArr, "%u", var);
		}
		//signed longs and long longs
		else if constexpr (std::is_same<T, long int>::value || std::is_same<T, long long int>::value ||
			//const versions
			std::is_same<T, const long int>::value || std::is_same<T, const long long int>::value) {
			
			sprintf(formArr, "%lld", (long long int)var);
		}
		//unsigned long and long longs
		else if constexpr (std::is_same<T, unsigned long int>::value || std::is_same<T, unsigned long long int>::value ||
			//const versions
			std::is_same<T, const unsigned long int>::value || std::is_same<T, const unsigned long long int>::value) {
			
			sprintf(formArr, "%llu", (unsigned long long int)var);
		}
		
		//float types:
		// default precision(6), use sprintf on your array and writeStrToFile manually
		
		//float and double
		else if constexpr (std::is_same<T, float>::value || std::is_same<T, double>::value ||
			//const versions
			std::is_same<T, const float>::value || std::is_same<T, const double>::value) {

			sprintf(formArr, "%f", var);
		}
		else if constexpr (std::is_same<T, long double>::value ||
			//const versions
			std::is_same<T, const long double>::value) {

			sprintf(formArr, "%L", var);
		}
		//none above applies, custom type
		else{
			writeDataToFile_Ref(var);
			printf("None of the types apply!\n");
			return;
		}
		writeStrToFile(formArr, false, useAutoFlush);
	}

	/// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>
	inline bool openFile(const std::string& str, const unsigned char flags = AEFW_FLAG_APPEND){
		return this->openFile(str.c_str(), flags);
	}
	 /// <summary>
	/// Open(and create) file with given name and flag
	/// </summary>
	/// <param name="filename">Name of the file, with extension</param>
	/// <param name="flags">Flags for file opening, AEFW_FLAG_* macros. More info in the docs</param></param>
	/// <returns>True if file is open(by currently used writer), false otherwise</returns>

	bool openFile(const char* str, const unsigned char flags = AEFW_FLAG_APPEND){
		if(strlen(str)){
			m_filename = str;
			switch(flags){


				case AEFW_FLAG_APPEND://cursor at end, allow changing cursor position
					openFileDirectly(str, "r+b");
					if(!m_filestr){//couldnt open, force create
						openFileDirectly(str, "wb");
					}
					fseek(m_filestr, 0L, SEEK_END);
					break;

				case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
					openFileDirectly(str, "ab");
					break;


				case AEFW_FLAG_NOFLAGS://no flags, defaulting to truncation
				case AEFW_FLAG_TRUNCATE://truncate file
					openFileDirectly(str, "wb");
					break;

				default:

					break;


			}
			if(!m_filestr){//file is still somehow nonexistent
				return false;
			}
			return true;
		}
		else{
			m_filename.clear();
			return false;
		}

	}
	
	///Flushes the opened file. That's it.
	inline void flushFile() const{
		if(m_filestr)
			fflush(m_filestr);
	}

	///close current file. return 0 if closed
	inline bool closeFile(){
		if(m_filestr)
			fclose(m_filestr);
		m_filestr = nullptr;
		return bool(this->m_filestr);//null if closed, something other if opened
	}

	///checks if file is open, true if it is, false otherwise
	inline bool isOpen() const{
		return bool(this->m_filestr);//null if closed, something other if opened
	}
	
	///returns last error of writer
	inline unsigned char getLastError() const{
		return this->m_lastError;
	}
	
	///returns name of currently open file
	inline std::string getFileName() const{
		return this->m_filename;
	}
	
	/// Interval in file writings before flush.
	/// Set to 1 -- flush every time;
	/// Set to 0 -- never auto-flush
	size_t m_autoflushInterval;
private:
	/// <summary>
	/// opens file with given name and flags
	/// needed so i dont need to retype ifdef for each
	/// of file openings
	/// cleaner
	/// </summary>
	/// <param name="fname"></param>
	/// <param name="flags"></param>
	inline void openFileDirectly(const char* fname, const char* flags) {
		//safety, so our compiler shut ups about the unsafe and deprecated function
		//and trigger only on vc++

// our compiler is vc++
#ifdef _MSC_VER 
		fopen_s(&m_filestr, fname, flags);
#else
		m_filestr = fopen(fname, flags);
#endif // _MSC_VER 

	}

	///full filename
	std::string m_filename;
	///counter for file entries written by this writer
	size_t m_writtenEntries;
	///object for file writing
	FILE* m_filestr;
	///writer's error indicator
	///Values are AEFW_ERR_* macros
	///More info in the docs
	unsigned char m_lastError;

};

#endif //!_AEFILEWRITER_HPP



