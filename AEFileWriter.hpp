#ifndef _AEFILEWRITER_HPP
#define _AEFILEWRITER_HPP

#include <cstring>
#include <string>
#include <vector>
#pragma warning(disable : 4996)

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
	/// <param name="af_interval">interval between automatic file flushing in file writes</param>
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
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
    inline void writeStrToFile(const std::string& str, const bool useAutoFlush = true){
        this->writeDataToFile_Ptr(str.c_str(), str.size(), sizeof(char), useAutoFlush);
    }

    /// <summary>
    /// Write std::vector of char (string), as is, to file, and flush if necessary
    /// </summary>
    /// <param name="str">String(in form of vector<char>) to write</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
    inline void writeStrToFile(const std::vector<char>& str, const bool useAutoFlush = true){
        this->writeDataToFile_Ptr(str.data(), str.size(), sizeof(char), useAutoFlush);
    }


    /// <summary>
    /// Write c-string, as is, to file, and flush if necessary
    /// </summary>
    /// <param name="cdata">С-String to write</param>
    /// <param name="dcount">length of string</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
    inline void writeStrToFile(const char* cdata, const bool useAutoFlush = true){
        this->writeDataToFile_Ptr(cdata, strlen(cdata), sizeof(char), useAutoFlush);
    }


    /// <summary>
    /// Write binary data, as is, to file, and flush if necessary
    /// Difference with writeDataToFile_Ref(): uses (const) pointer to the variable/data
    /// Doesn't work(fine) with literals
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
    /// Write binary data, as is, to file, and flush if necessary
    /// Difference with writeDataToFile_Ptr(): uses (const) reference to the variable
    /// Which works with literals
    /// </summary>
    /// <param name="cdata">(pointer to) object to write</param>
    /// <param name="tsize">size of the variable, default is the sizeof()</param>
	/// <param name="useAutoFlush">Flag to use automatic file flushing each n writes specified in autoflush_interval</param>
    template<typename T>
    void writeDataToFile_Ref(const T& cdata, const std::size_t tsize = sizeof(T), const bool useAutoFlush = true){
        if(isOpen()){
            fwrite(&cdata, sizeof(unsigned char), tsize, m_filestr);
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

    template<typename T>
    inline void writeToFile(const T& var, const bool useAutoFlush = true){
        //char array for formatting
        //classic 4*size of the variable
        char formArr[sizeof(var)];
        
        //great line of if's to check the types
        if constexpr(std::is_same<T, char>::value || std::is_same<T, unsigned char>::value){
            //the type is well, char
            
        }
        else if constexpr(std::is_same<T, short>::value || std::is_same<T, unsigned short>::value){
            //the type is well, char
            sprintf(formArr, "%d", var);
        }
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
                    m_filestr = fopen (str,"r+b");
                    if(!m_filestr){//couldnt open, force create
                        m_filestr = fopen (str,"wb");
                    }
                    fseek(m_filestr, 0L, SEEK_END);
                    break;

                case AEFW_FLAG_APPEND_NO_CURSOR_MOVE://only appending, not allowing to change cursor position
                    m_filestr = fopen (str,"ab");
                    break;

                case AEFW_FLAG_TRUNCATE://truncate file
                    m_filestr = fopen (str,"w");
                    break;

                case AEFW_FLAG_NOFLAGS://no flags
                    //following fstream
                    m_filestr = fopen (str,"wb");
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



