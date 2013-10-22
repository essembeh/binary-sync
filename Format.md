## Nota
    byte    = 1 byte
    uint8   = 1 byte
    uint16  = 2 bytes
    uint32  = 4 bytes 
    uint64  = 8 bytes 

---

# HEADER  

### DESCRIPTION
Used to identify the file, it contains informations about the original file, its size, the block size used to compute hashes or transmit data.  
It can also contain extra informations for example to identify the target file.  
The BLOCK_SIZE must be equal along the workflow, to compare two checksum files, to generate data file, to apply data.  
The only checksum method implemented for now is Adler32, the size of a hach is 4 bytes.

### FIELDS
    byte[2]    "BS"            arbitrary string
    uint8      VERSION         version of the binary format
    uint8      TYPE            'C': checksum file
                               'D': data file
    uint64     TOTAL_SIZE      the size of the master file (unused for a snapshot request)
    uint64     BLOCK_SIZE      the size used for blocks for hash computing or data.
    byte[32]   USER_DATA       can be used to share extra info between remote and master
    
### EXAMPLE
    BS 1 20 1024 32 10 /dev/sda1: 
> is a header of a checksum file, version 1, of a 1kb file, using 32 bytes block, with user date "/dev/sda1"

--- 

# FOOTER  

### DESCRIPTION
To identify the end of file

### FIELDS
    uint64     ITEM_COUNT      number of items on the file
    byte[2]    "BS"            arbitrary string

---

# CHECKSUM FILE 

### DESCRIPTION
Contains hash of requested blocks.  
The size of hash depends on the hash function used.

### FIELDS
    sizeof(hash)   HASH          hash of block 0
    sizeof(hash)   HASH          hash of block 1
    [...]
    sizeof(hash)   HASH          hash of last block 
    
### EXAMPLE
    65017 65018 65018 65019 ... 65042
> Hash of block 0 is 65003  
> Hash of block 1 is 65017  
> Hash of block 2 is 65018  
> Hash of block 3 is 65019  
> ...  
> Hash of last block is 65042  

---

# DATA FILE

### DESCRIPTION
Contains data of requested blocks.

### FIELDS
    uint64             BLOCK_ID
    byte[BLOCK_SIZE]   DATA            data of block
    ... (sequence can be repeated)
    
### EXAMPLE
    3 0x54520000000000000001 17 0x54520000000000000002 42 0x54520000000000000003 ... 59 0x54520000000000000009
> Data of block  3 is 0x54520000000000000001  
> Data of block 17 is 0x54520000000000000002  
> Data of block 42 is 0x54520000000000000003  
> ...  
> Data of block 59 is 0x54520000000000000009  

