package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.List;

/**
 * CompFile
 * Represents a file (document/directory) found in CVFS
 * @author Lammy
 */
abstract public class CompFile {
    private String fileName;
    private FileType fileType;
    private CompFile parentFile;
    private List <CompFile> subFile;

    /**
     * Constructor of CompFile
     * @param fileName file name
     * @param fileType file type (e.g. css, txt, html and java)
     *                 represented by enum FileType
     * @param parentFile the parent of the current CompFile (Directory / Disk)
     * @param subFile the files in the current CompFile (Directory / Disk)
     *                represented by a List (ArrayList) of CompFile
     */
    public CompFile (String fileName, FileType fileType, CompFile parentFile, List<CompFile> subFile) {
        this.fileName = fileName;
        this.fileType = fileType;
        this.parentFile = parentFile;
        this.subFile = subFile;
    }

    /* Utility method */
    /**
     * @return the size (to be overridden by subclass method)
     */
    public int getSize() {
        return 0;
    }

    /**
     * @return the string representation (to be overridden by subclass method)
     */
    public String toString () {
        return null;
    }


    /* Getter */
    /**
     * @return file name
     */
    public String getFileName () {
        return this.fileName;
    }

    /**
     * @return file type
     */
    public FileType getFileType () {
        return this.fileType;
    }

    /**
     * @return the parent directory / disk reference
     */
    public CompFile getParentFile () {
        return this.parentFile;
    }

    /**
     * @return the CompFile stored in this CompFile
     */
    public List <CompFile> getSubFile () {
        return this.subFile;
    }

    /* Setter */
    /**
     * @param fileName file name
     */
    public void setFileName (String fileName) {
        this.fileName = fileName;
    }

}
