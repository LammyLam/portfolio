package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;

/**
 * Disk is an extension of CompFile
 * Represents a virtual disk
 * @author Lammy
 */
public class Disk extends CompFile {
    // How much file can a disk hold
    private int diskCapacity;
    // How much file the disk is holding
    private int diskLoadedSize;

    /**
     * Constructor of Disk
     * @param diskName disk name
     * @param diskCapacity maximum size of files that can be stored in the disk
     */
    public Disk (String diskName, int diskCapacity) {
        super(diskName, FileType.DISK, null, new ArrayList<CompFile>());
        this.diskCapacity = diskCapacity;
        this.diskLoadedSize = 0;
    }

    /**
     * addSize adds a file size to the loaded size
     * @param fileSize file size
     */
    public void addSize (int fileSize) {
        this.diskLoadedSize += fileSize;
    }


    /**
     * deductSize subtract a file size from the loaded size
     * @param fileSize file size
     */
    public void deductSize (int fileSize) {
        this.diskLoadedSize -= fileSize;
    }


    /**
     * canAddCompFile check if the disk can add a CompFile
     * @param fileSize file size
     * @return true or false
     */
    public boolean canAddCompFile (int fileSize) {
        if (diskLoadedSize + fileSize <= diskCapacity) {
            return true;
        }
        return false;
    }

    /**
     * Overrides superclass toString()
     * @return a string in the format "xxxxx.type - Size: xxx
     */

    public String toString () {
        return this.getFileType() + this.getFileName() + " - Size: " + this.diskLoadedSize;
    }

    @Override
    /**
     * @return the size of all files in the disk
     */
    public int getSize () {
        return this.diskLoadedSize;
    }

    /**
     * @return the capacity of this disk
     */
    public int getCapacity () {
        return this.diskCapacity;
    }
}
