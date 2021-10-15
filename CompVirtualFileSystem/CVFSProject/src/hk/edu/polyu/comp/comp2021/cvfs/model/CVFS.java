package hk.edu.polyu.comp.comp2021.cvfs.model;

/**
 * CVFS Model - Group 18
 * @author Lammy
 * This model part of the MVC pattern only handles the manipulation and logic of disks, documents and directories
 * Error-checking on logic error, such as directory or criterion not found, is done here
 * Error-checking on user input is implemented in the "Controller" of CVFS
 */

public class CVFS {
    // The number of disk created in the current CVFS session
    private int diskNumber;
    // The disk that is being currently accessed
    private Disk currentDisk = null;
    // The directory that the user is currently in
    private CompFile currentDirectory = null;
    // The base size of each file
    private final static int BASESIZE = 40;
    // The HashMap of Criteria
    private CriteriaMap criteriaMap;

    /**
     * Constructor of the class CVFS
     * Set diskNumber (the number of disk used in current CVFS session) to 0
     */
    // Constructor of the class CVFS
    public CVFS() {
        // Instanitiate criteriaMap
        criteriaMap = new CriteriaMap();
        criteriaMap.addCriteria("IsDocument", new Criteria());
        diskNumber = 0;
    }

    /**
     * REQ1 - Creating a disk with a specified maximum size (in bytes)
     * @param diskCapacity the maximum size of files that can be stored in this disk
     * @return a status to CVFSView
     *         Status.NEWDISK = the first disk of the current session is successfully created
     *         Status.CLOSEANDNEWDISK = the current disk is closed and the a new disk is created
     */
    public Status createDisk(int diskCapacity) {
        // Disk name is trivial until the "store" command is used
        String diskName = "Disk" + diskNumber;
        // Create the disk with specified size
            Disk disk = new Disk(diskName, diskCapacity);

        if (currentDisk == null) {
            // Set current disk and current directory to the newly-created disk
            currentDisk = disk;
            currentDirectory = disk;
            diskNumber++;
            return Status.NEWDISK;
        }
        currentDisk = disk;
        currentDirectory = disk;
        diskNumber++;
        return Status.CLOSEANDNEWDISK;
    }

    /**
     * REQ2 - Creating a document with a specified name, document type and content
     * @param docName document name
     * @param docType document type (e.g. css, txt, html, java)
     * @param docContent document content (can be anything)
     * @return a status to CVFSView
     *         Status.NEWDOC = a new document is successfully created
     *         Status.FILETOOBIG = the disk cannot hold the document to be created, creation is aborted
     *         Status.HASSAMEFILE = the current directory contains file of the same name, creation is aborted
     *         Status.NODISK = no disk is created to hold the document, creation is aborted
     */
    public Status createDocument(String docName, String docType, String docContent) {
        // Condition of Creation: No document or directory with the same name existing in the same directory
        // AND the disk have enough space to hold the current file
        // AND there is a disk operating
        if (currentDisk != null) {
            if (getSameCompFile(docName) == null) {
                if (currentDisk.canAddCompFile(BASESIZE + docContent.length() * 2)) {
                    new Document(docName, currentDirectory, docType, docContent);
                    currentDisk.addSize(BASESIZE + docContent.length() * 2);
                    return Status.NEWDOC;
                } else {
                    // Size > load size
                    return Status.FILETOOBIG;
                }
            } else {
                // getSameCompFile(docName) == null
                return Status.HASSAMEFILE;
            }
        }
        else{
            // currentDisk == null
            return Status.NODISK;
        }
    }

    /**
     * REQ3 - Creating a directory with a specified name
     * @param dirName directory name
     * @return a status to CVFSView
     *         Status.NEWDIR = a new directory is successfully created in the current directory
     *         Status.FILETOOBIG = the disk cannot hold the directory to be created, creation is aborted
     *         Status.HASSAMEFILE = the current directory contains file of the same name, creation is aborted
     *         Status.NODISK = no disk is created to hold the directory, creation is aborted
     */
    public Status createDirectory (String dirName) {
        // Condition of Creation: No document or directory with the same name existing in the same directory
        // AND the disk have enough space to hold the current directory
        // AND there is a disk operating
        if (currentDisk != null) {
            if ((getSameCompFile(dirName) == null)) {
                if (currentDisk.canAddCompFile(BASESIZE)) {
                    new Directory(dirName, currentDirectory);
                    currentDisk.addSize(BASESIZE);
                    return Status.NEWDIR;
                }
                else {
                    // Size > loaded size
                    return Status.FILETOOBIG;
                }
            } else {
                // (getSameCompFile(dirName) != null)
                return Status.HASSAMEFILE;
            }
        }
        else {
            // currentDisk == null
            return Status.NODISK;
        }
    }

    /**
     * REQ4 - Deleting a file (Document/Directory) with a specified name
     * @param fileName file name (of the file to be deleted)
     * @return a status to CVFSView
     *         Status.DELETEFILE = the corresponding file is successfully deleted
     *         Status.NOSUCHFILE = the corresponding file does not exist in the current directory, deletion is aborted
     *         Status.NODISK = no disk is created to handle the deletion, deletion is aborted
     */
    public Status deleteFile (String fileName) {
        if (currentDisk != null) {
            if (this.getSameCompFile(fileName) != null) {
                currentDisk.deductSize(this.getSameCompFile(fileName).getSize());
                currentDirectory.getSubFile().removeIf(e -> (e.getFileName().equals(fileName)));
                return Status.DELETEFILE;
            }
            else {
                // this.getSameCompFile(fileName) == null
                return Status.NOSUCHFILE;
            }
        }
        else {
            // currentDisk == null
            return Status.NODISK;
        }
    }

    /**
     * REQ5 - Renaming an existing file with a specified name
     * @param oldName the name of the file to be renamed
     * @param newName the new name of the file
     * @return a status to CVFSView
     *         Status.RENAMEFILE = the file is successfully renamed
     *         Status.NOSUCHFILE = the corresponding file does not exist in the current directory, rename is aborted
     *         Status.NODISK = no disk is created to handle the rename, rename is aborted
     */
    public Status renameFile (String oldName, String newName) {
        if (currentDisk != null) {
            CompFile targetDocument = getSameCompFile(oldName);
            if (targetDocument != null) {
                if (getSameCompFile(newName) == null) {
                    targetDocument.setFileName(newName);
                    return Status.RENAMEFILE;
                }
                else {
                    return Status.HASSAMEFILE;
                }
            }
            else {
                return Status.NOSUCHFILE;
            }
        }
        else {
            // currentDisk == null
            return Status.NODISK;
        }
    }

    /**
     * REQ6 - Changing the directory to the target directory
     *        The target directory must be either ".." or any directory in the current directory
     * @param targetName the name of the target directory
     * @return a status to CVFSView
     *         Status.GOPARENT = the current directory is successfully set as the parent of the current directory
     *         Status.NOPARENT = cannot set the parent of the current directory as the current directory, directory change is aborted
     *         Status.CHANGEDIR = the current directory is successfully set as the target directory
     *         Status.NOTDIR = the file with the same name is found, but it is not a directory, directory change is aborted
     *         Status.NOSUCHFILE = the corresponding directory does not exist in the current directory, directory change is aborted
     *         Status.NODISK = no disk is created to handle the directory change, directory change is aborted
     */
    public Status changeDirectory (String targetName) {
        if (currentDisk != null) {
            if (targetName.equals("..")) {
                if (currentDirectory.getParentFile() != null) {
                    currentDirectory = currentDirectory.getParentFile();
                    return Status.GOPARENT;
                }
                else {
                    return Status.NOPARENT;
                }
            }
            else {
                CompFile targetDirectory = getSameCompFile(targetName);
                if (targetDirectory != null) {
                    if (targetDirectory.getFileType() == FileType.DIRECTORY) {
                        currentDirectory = targetDirectory;
                        return Status.CHANGEDIR;
                    }
                    else {
                        //!(targetDirectory.getFileType().equals(FileType.DIRECTORY))
                        return Status.NOTDIR;
                    }
                }
                else {
                    // targetDirectory == null
                    return Status.NOSUCHFILE;
                }
            }
        }
        else {
            // currentDisk == null
            return Status.NODISK;
        }
    }

    /**
     * REQ9 - newSimpleCri creates a simple criterion for searching for user input
     * @param criName criterion name
     * @param attrName attribute name
     * @param op operator
     * @param val value
     * @return a status:
     *         Status.NEWCRI = a new criteria is successfully created
     *         Status.HASSAMECRI = a criterion with same name is created, criterion creation is aborted
     *         Status.NODISK = there is no disk in the current CVFS session, criterion creation is aborted
     */
    public Status newSimpleCri (String criName, String attrName, String op, String val) {
        if (!criteriaMap.hasSameCriteria(criName)) {
            criteriaMap.addCriteria(criName, new Criteria(criName, attrName, op , val));
            return Status.NEWCRI;
        }
        else {
            // criteriaMap.hasSameCriteria(criName)
            return Status.HASSAMECRI;
        }
    }

    /**
     * REQ11 - newNegation creates a negated criterion from a given criterion
     * @param retName the name of the negated criterion
     * @param storeName the name of the criterion to be negated
     * @return a status:
     *         Status.NEWNEGATION = a negated criterion is successfully created
     *         Status.NOSUCHCRI = the criterion to be negated is not found, negation is aborted
     *         Status.HASSAMECRI = the name of the resulting criterion already exists, negation is aborted
     */
    public Status newNegation (String retName, String storeName) {
        if(!criteriaMap.hasSameCriteria(retName)) {
            if (criteriaMap.hasSameCriteria(storeName)) {
                criteriaMap.addCriteria(retName, new Criteria(retName, criteriaMap.getCriteria(storeName)));
                return Status.NEWNEGATION;
            }
            else {
                // !criteriaMap.hasSameCriteria(storeName)
                return Status.NOSUCHCRI;
            }
        }
        else {
            // criteriaMap.hasSameCriteria(retName)
            return Status.HASSAMECRI;
        }
    }

    /**
     * REQ11 - newBinaryCri creates a composite criterion from two existing criteria
     * @param retName the name of composite criterion
     * @param criteria1 the name of the first existing criterion (either simple or composite)
     * @param logicOp logic operator
     * @param criteria2 the name of the second existing creiterion (either simple or composite)
     * @return a status:
     *                 Status.NEWBINARYCRI = a new binary composite criterion is successfully created
     *                 Status.NOSUCHCRI = there is no existing criteria with given name, binary criterion creation is aborted
     *                 Status.HASSAMECRI = there is already a criterion with the same name, binary criterion creation is aborted
     */
    public Status newBinaryCri (String retName, String criteria1, String logicOp, String criteria2) {
        if(!criteriaMap.hasSameCriteria(retName)) {
            if (criteriaMap.hasSameCriteria(criteria1) && criteriaMap.hasSameCriteria(criteria2)) {
                criteriaMap.addCriteria(retName, new Criteria(retName, criteriaMap.getCriteria(criteria1), logicOp, criteriaMap.getCriteria(criteria2)));
                return Status.NEWBINARYCRI;
            }
            else {
                // (criteriaMap.hasSameCriteria(criteria1) == false ||  (criteriaMap.hasSameCriteria(criteria2) == false
                return Status.NOSUCHCRI;
            }
        }
        else {
            // criteriaMap.hasSameCriteria(retName)
            return Status.HASSAMECRI;
        }
    }

    /* Utility Methods */
    /* Getter */
    /**
     * getSameCompFile returns either the CompFile with the same name, or "null"
     * @param fileName name of the file to be serached
     * @return a corresponding CompFile reference or null
     */
    public CompFile getSameCompFile (String fileName) {
        if (currentDirectory != null && currentDisk != null) {
            for (CompFile checkFile : currentDirectory.getSubFile()) {
                if (checkFile.getFileName().equals(fileName)) {
                    return checkFile;
                }
            }
            return null;
        }
        return null;
    }

    /**
     * Wrapper of getCurrentPath (CompFile compFile)
     * @return result of getCurrentPath (this.currentDirectory)
     */

    public String getCurrentPath () {
        if (currentDirectory != null) {
            return getCurrentPath(this.currentDirectory);
        }
        return "";
    }

    /**
     * getCurrentPath concatenates file names create a path name
     * this will be shown in the same line before the command line input
     * @param compFile the current directory reference (will change according to the recursion)
     * @return a path name from current directory to the root (Disk)
     */
    public String getCurrentPath (CompFile compFile) {
        if (compFile.getParentFile() != null) {
            return (getCurrentPath(compFile.getParentFile()) + "\\" + compFile.getFileName());
        }
        else {
            return compFile.getFileName();
        }
    }

    /**
     * @return the current Disk reference
     */
    public Disk getCurrentDisk () {
        return this.currentDisk;
    }

    /**
     * @return the current directory reference
     */
    public CompFile getCurrentDirectory () {
        return this.currentDirectory;
    }

    /**
     * @return the criteria map of the current CVFS session
     */
    public CriteriaMap getCriteriaMap () {
        return this.criteriaMap;
    }

}
