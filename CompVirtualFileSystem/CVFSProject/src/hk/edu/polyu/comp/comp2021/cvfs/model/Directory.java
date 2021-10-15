package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;

/**
 * Directory is an extension of CompFile
 * represents a directory in a disk
 * @author Lammy
 */
public class Directory extends CompFile{

    /**
     * BASESIZE is the "base" size of all directory
     * Since all directory, initially, will have size of 40 (size of itself)
     */
    public static final int BASESIZE = 40;


    /**
     * Constructor of Directory
     * @param dirName directory name
     * @param parentFile the parent of this directory
     */
    public Directory (String dirName, CompFile parentFile) {
        super(dirName, FileType.DIRECTORY, parentFile, new ArrayList<>());
        parentFile.getSubFile().add(this);
    }

    /**
     * Overrides superclass's getSize
     * @return the return value of updateDirSize
     */
    @Override
    public int getSize () {
        return updateDirSize(this);
    }

    /**
     * updateDirSize recursively counts the size of files in this directory
     * @param compFile the directory to be counted
     * @return the total size of all files in the directory
     */
    public int updateDirSize (CompFile compFile) {
        int tempSum = BASESIZE;
        for (CompFile loopFile : compFile.getSubFile()) {
            if (loopFile.getFileType() == FileType.DOCUMENT) {
                tempSum += loopFile.getSize();
            }
            if (loopFile.getFileType() == FileType.DIRECTORY) {
                tempSum += updateDirSize(loopFile);
            }
        }
        return tempSum;
    }

    /**
     * overrides superclass's toString
     * @return a string in the format "DIRECTORY xxxxxxx - Size: "
     */
    public String toString () {
        return this.getFileType() + " " + this.getFileName() + " - Size: " + getSize();
    }

}
