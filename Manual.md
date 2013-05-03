# Manual for binary-sync tools

---
Here are the convetion fot file extensions

* **.ckr** will represent a **checksum request**
* **.ckf** will represent a **checksum file**
* **.dtr** will represent a **data request**
* **.dtf** will represent a **data file**


---

# bs-info

### NAME
>     bs-info - Display informations about a file

### SYNOPSIS
>     bs-info FILE

### DESCRIPTION
> Display informations of the given file, its header, its data.

### USAGE
>     bs-info sda.bpr
>> Display informations about the sda.bsr file


---

# bs-checksum

### NAME
>     bs-checksum - Create a checksum file, ie a list of blocks checksums

### SYNOPSIS
>     bs-checksum [-b SIZE] [-u TEXT] -o FILE SOURCE

### DESCRIPTION

>     -b SIZE, --block-size=SIZE
>> Used SIZE as block size, cannot be used if a snapshot request is provided with -i


>     -o FILE, --output=FILE 
>> Path to the generated file, the snapshot

>     -u TEXT, --user-data=TEXT
>> Write or overide user data with TEXT. 

>     SOURCE
            The file to process

### USAGE
>     bp-checksum \
>         --output=checksum.ckf \
>         --block=4096 
>         --user-data="my disk: /dev/sda" \
>         /dev/sda
>> Creates a **checksum file** for file /dev/sda whith checksums of all 4k blocks.


---
# bs-data-request 

### NAME
>     bs-data-request - Create a data request

### SYNOPSIS
>     bp-data-request -f FILE -t FILE -o FILE [-u TEXT] 

### DESCRIPTION
>     -f FILE, --from=FILE
>> The *remote* **checksum file**

>     -t FILE, --to=FILE
>> The *master* **checksum file**

>     -o FILE, --output=FILE 
>> Path of the generated **data request** file

>     -u TEXT, --user-data=TEXT
>> Overwrite user data with TEXT

        
### USAGE
>     bs-data-request \
>         --from=remote.ckf \
>         --to=master.ckf \
>         --output=request.dtr
>> Creates a **data request** file to synchronise the file from the *remote* version to the *master* version.

---
# bs-data

### NAME
>     bs-data - Create a data file
### SYNOPSIS
>     bp-data -i FILE -o FILE [-u TEXT] 

### DESCRIPTION
>     -i FILE, --input=FILE
>> The **data request** containing the list of blocks

>     -o FILE, --output=FILE
>> The **data file** destination

>     -u TEXT, --user-data=TEXT
>> Overwrite user data with TEXT

        
### USAGE
>     bs-data \
>         --input=request.dtr \
>         --output=data.dtf 
>> Creates a **data file** containing all blocks from the given _request.dtr_

