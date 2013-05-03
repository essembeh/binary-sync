# Presentation

*binary-sync* is a collection of tools to synchronize large binary files between distant systems.  
There are two key ideas:

* *synchronise*: we are trying to minimize the amount of exchanged data between two systems to get the exact same file
* *large file*: We only have a local copy of the file on each system. As files are potentially large (or potentially block devices), a system cannot have two version of a it.

# How it works

It works by exchanging messages between a *master* and a *remote*.  
Supposing the *master* has the version B and the *remote* has the version A, you want the *remote* to get the version B.

1. (Optionnal) The *remote* can ask the *master* for a specific **checksum file**, with given block size, a specific range of blocks ...
2. The *master* makes a **checksum file** 
3. The *remote* builds its own **checksum file**
4. Either the *remote* or the *master* can make a **data request**, the list of blocks having different checksum
5. The *master* create a **data file** containing all blocks
6. Finally, the *remote* can write blocks from the **data file** to get the new verion of the file

Here are the convetion fot file extensions

* **.ckr** will represent a **checksum request**
* **.ckf** will represent a **checksum file**
* **.dtr** will represent a **data request**
* **.dtf** will represent a **data file**

The format of each file is documented [here](Format.md).


# Alternatives

Some tools already exists to manage diff and patch on binary files

* xdelta [site](http://xdelta.org/)
* bsdiff [site](http://www.daemonology.net/bsdiff/)

They work using the two versions of the file and finding differences to generate a *patch*. Once you have the *patch* file, anyone with the version A or B of the file can get the other one.  



### Pros

* Unlike these tools, with *binary-sync* you do not need the two versions of the file on a system to find the differences and synchronise it. 
* The memory footprint is very low unlike traditionnal diff tools because there is no difference search algorithm.

### Cons


* On the other side, there is no difference search algorithm and therefore you may request a block even if only one byte difference.
* Another limitation is that the file on *master* should not be updated between the **checksum file** generation and the **data file** creation to guarantee the data coherence. 
* As none of the systems have the two versions of the file, they need to communicate at least twice to synchronise it, one for the *remote* to send its **checksum file**, and when the *master* sends the **data file** (computed from its own **checksum file**, the *remote* one and the original file). 



# Using

See the [manual page](Manual.md) for a complete description of the binaries.



