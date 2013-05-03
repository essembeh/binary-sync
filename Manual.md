## Manual for binary-sync tools

---
Here are the convetion fot file extensions

* **.ckr** will represent a **checksum request**
* **.ckf** will represent a **checksum file**
* **.dtr** will represent a **data request**
* **.dtf** will represent a **data file**


---

# bs-info

### NAME
    bs-info - Display informations about a file

### SYNOPSIS
    bs-info FILE

### DESCRIPTION
Display informations of the given file, its header, its data.

### USAGE
    bs-info sda.ckr
> Display informations about the sda.ckr file


---

# bs-checksum

### NAME
    bs-checksum - Create a checksum file, ie a list of blocks checksums

### SYNOPSIS
    bs-checksum [-b SIZE] [-u TEXT] -o FILE_CKF SOURCE

### DESCRIPTION
Create a FILE_CKF **checksum file** which contains the checksum of each SOURCE file block of size SIZE.

### OPTIONS
    -b SIZE, --block-size=SIZE
        Used SIZE as block size

    -o FILE_CKF, --output=FILE_CKF 
        Path to the generated file, the snapshot

    -u TEXT, --user-data=TEXT
        Write or overide user data with TEXT. 

    SOURCE
        The file to process

### USAGE
    bp-checksum \
        --output=checksum.ckf \
        --block=4096 
        --user-data="my disk: /dev/sda" \
        /dev/sda
> Creates a **checksum file** for file /dev/sda whith checksums of all 4k blocks.


---
# bs-data-request 

### NAME
>     bs-data-request - Create a data request

### SYNOPSIS
>     bp-data-request -f FILE_CKF -t FILE_CKF -o FILE_DTR [-u TEXT] 

### DESCRIPTION
Create a FILE_DTR **data request** file from two FILE_CKF **checksum files**. 
It's the list of all blocks which have different checksum in the given **checksum files**.

### OPTIONS
    -f FILE_CKF, --from=FILE_CKF
        The *remote* **checksum file**

    -t FILE_CKF, --to=FILE_CKF
        The *master* **checksum file**

    -o FILE_DTR, --output=FILE_DTR 
        Path of the generated **data request** file

    -u TEXT, --user-data=TEXT
        Overwrite user data with TEXT

### USAGE
    bs-data-request \
        --from=remote.ckf \
        --to=master.ckf \
        --output=request.dtr
> Creates a **data request** file to synchronise the file from the *remote* version to the *master* version.

---
# bs-data

### NAME
    bs-data - Create a data file

### SYNOPSIS
    bp-data -i FILE_DTR -o FILE_DTF [-u TEXT] 

### DESCRIPTION
Create a FILE_DTF file containing the data of all given blocks from the FILE_DTR **data request** file.

### OPTIONS
    -i FILE_DTR, --input=FILE_DTR
        The **data request** containing the list of blocks

    -o FILE_DTF, --output=FILE_DTF
        The **data file** destination

    -u TEXT, --user-data=TEXT
        Overwrite user data with TEXT

### USAGE
    bs-data \
        --input=request.dtr \
        --output=data.dtf 
> Creates a **data file** containing all blocks from the given _request.dtr_

---
# bs-data-apply

### NAME
    bs-data-apply - Update a file with the given data blocks

### SYNOPSIS
    bp-data-apply -i FILE_DTF -d FILE

### DESCRIPTION
Modify the file with the given blocks contained in the **data file**.

### OPTIONS
    -i FILE_DTF, --input=FILE_DTF
        The **data file** containing the needed block data

    -d FILE, --destination=FILE
        The file to modify
        
### USAGE
    bs-data-apply \
        --input=data.dtf \
        --destination=/dev/sda
> Modify the file /dev/sda with the given blocks

