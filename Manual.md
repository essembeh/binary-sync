## Manual for binary-sync tools

---
Here are the conventions for file extensions:

* **.sum** will represent a **checksum file**
* **.data** will represent a **data file**


---

# bs-info

### NAME
    bs-info - Display informations about a file

### SYNOPSIS
    bs-info FILE

### DESCRIPTION
Display informations of the given file, its header, its data.

### USAGE
    bs-info sda.sum
> Display informations about the sda.sum file


---

# bs-checksum

### NAME
    bs-checksum - Create a checksum file, ie a list of blocks checksums

### SYNOPSIS
    bs-checksum [-b SIZE] [-u TEXT] -o FILE_SUM SOURCE

### DESCRIPTION
Create a FILE_SUM **checksum file** which contains the checksum of each SOURCE file block of size SIZE.

### OPTIONS
    -b SIZE, --block-size=SIZE
        Used SIZE as block size

    -o FILE_SUM, --output=FILE_SUM 
        Path to the generated file, the snapshot

    -t FILE, --target=FILE
        Path to the target file which will be processed

    -u TEXT, --user-data=TEXT
        Write or overide user data with TEXT.

### USAGE
    bp-checksum \
        --output=sda.sum \
        --block=4096 
        --user-data="my disk: /dev/sda" \
        --target=/dev/sda
> Creates a **checksum file** for file /dev/sda whith 4k blocks.


---
# bs-data 

### NAME
>     bs-data - Create a data file

### SYNOPSIS
>     bp-data -f FILE_SUM -t FILE_SUM -o FILE_DATA [-u TEXT] 

### DESCRIPTION
Create a FILE_DATA **data file** file from two FILE_SUM **checksum files**. 
The **data file** contains all blocks which checksum if different between **from** and **to** versions.

### OPTIONS
    -r FILE_SUM, --remote=FILE_SUM
        The *remote* **checksum file**

    -m FILE_SUM, --master=FILE_SUM
        The *master* **checksum file**

    -t FILE, --target=FILE
        Path to the target file which will be processed

    -o FILE_DTR, --output=FILE_DTR 
        Path of the generated **data file**

    -u TEXT, --user-data=TEXT
        Overwrite user data with TEXT

### USAGE
    bs-data-request \
        --remote=remote.sum \
        --master=master.sum \
        --target=/dev/sda1 \
        --output=update.data
> Creates a **data file** to synchronise the file from the *remote* version to the *master* version.

---
# bs-apply

### NAME
    bs-apply - Update a file with the given data blocks

### SYNOPSIS
    bp-apply -i FILE_DATA -d FILE

### DESCRIPTION
Modify the file with the given blocks contained in the **data file**.

### OPTIONS
    -d FILE_DATA, --data=FILE_DATA
        The **data file** containing the needed block data

    -t FILE, --target=FILE
        Path to the target file which will be processed
        
### USAGE
    bs-apply \
        --data=update.data \
        --target=/dev/sda
> Modify the file /dev/sda with the given blocks

