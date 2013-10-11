## Nota
- byte    = 1 byte
- uint8   = 1 bytes
- uint16  = 2 bytes
- uint32  = 4 bytes 

---

# HEADER  

### DESCRIPTION
Used to identify the file, it contains informations about the original file, its size, the block size used to compute hashes or transmit data.  
It can also contain extra informations for example to identify the file.  
The header should be the same along all the process, from the snapshot request to the patch (user data are not updated by default).
Hash size depends on the hash function used
* size of hash with Adler32: 4 bytes 
* size of hash with CRC32: 4 bytes

### FIELDS
    uint8     VERSION          version of the binary format
    uint8     TYPE             10: checksum file
                               20: data file
    uint32     TOTAL_SIZE      the size of the master file (unused for a snapshot request)
    uint32     BLOCK_SIZE      the size used for blocks for hash computing or data.
    uint8      HASH_FUNCTION   'A': Adler32, 'C': CRC32
    uint32     USER_DATA_LEN   can be 0 if no user data
    byte[n]    USER_DATA       can be used to share extra info between remote and master
    
### EXAMPLE
    1 20 1024 32 10 /dev/sda1: 
> is a header of a checksum file, version 1, of a 1kb file, using 32 bytes block, with user date "/dev/sda1"

--- 

# FOOTER  

### DESCRIPTION
To identify the end of file

### FIELDS
    byte[4]    "FOOT"          arbitrary string

---

# CHECKSUM FILE 

### DESCRIPTION
Contains hash of requested blocks.  
The size of hash depends on the hash function used.

### FIELDS
    uint32         START_INDEX
    uint32         START_INDEX 
    sizeof(hash)   HASH          hash of block START_INDEX
    sizeof(hash)   HASH          hash of block START_INDEX+1
    [...]
    sizeof(hash)   HASH          hash of block STOP_INDEX
    ... (sequence can be repeated)
    
### EXAMPLE
    3 3 65003 17 42 65017 65018 65018 65019 ... 65042
        Hash of block 3 is 65003
        Hash of block 17 is 65017
        Hash of block 18 is 65018
        Hash of block 19 is 65019
        ...
        Hash of block 42 is 65042

---

# DATA FILE

### DESCRIPTION
Contains data of requested blocks.

### FIELDS
    uint32             START_INDEX
    uint32             START_INDEX
    byte[BLOCK_SIZE]   DATA            data of block START_INDEX
    byte[BLOCK_SIZE]   DATA            data of block START_INDEX+1
    [...]
    byte[BLOCK_SIZE]   DATA            data of block STOP_INDEX
    ... (sequence can be repeated)
    
### EXAMPLE
    3 3 0x54520000000000000003 17 42 0x54520000000000000017 0x54520000000000000018 0x54520000000000000019 ... 0x54520000000000000042
        Data of block 3 is 0x54520000000000000003
        Data of block 17 is 0x54520000000000000017
        Data of block 18 is 0x54520000000000000018
        Data of block 19 is 0x54520000000000000019
        ...
        Data of block 42 is 0x54520000000000000042

