package hk.edu.polyu.comp.comp2021.cvfs;

import hk.edu.polyu.comp.comp2021.cvfs.model.*;

/**
 * CVFS View
 * This view part of the MVC pattern outputs information to the console
 * By using Status and the given parameters
 * @author Lammy
 */
public class CVFSView {
    /**
     * BASEISIZE = the size of each directory (for recursive counting)
     */
    public static final int BASESIZE = 40;
    private CVFS cvfs;

    /**
     * Constructor of CVFS View
     * @param cvfs the cvfs instance
     */
    public CVFSView (CVFS cvfs) {
        this.cvfs = cvfs;
    }

    /**
     * feedbackCreateDisk outputs feedback according to the situation of disk creation
     * @param status status of disk creation
     * @param diskSize current size of the disk (total size of files loaded)
     */
    // Feedback of commands
    public void feedbackCreateDisk (Status status, String diskSize) {
        if (status == Status.NEWDISK) {
            System.out.println("A disk with size " + Integer.parseInt(diskSize) + " byte(s) is created.");
        }
        if (status == Status.CLOSEANDNEWDISK) {
            System.out.println("The current disk is closed. A new disk with size " + Integer.parseInt(diskSize) + " byte(s) is created.");
        }
    }

    /**
     * feedBackCreateDocument outputs feedback according to the situation of document creation
     * @param status status of document creation
     * @param docName document name
     * @param fileType file type
     */
    public void feedBackCreateDocument (Status status, String docName, String fileType) {
        switch (status) {
            case NODISK:
                System.out.println("No disk is being operated. Please create a disk");
                break;

            case HASSAMEFILE:
                System.out.println("A file (document/directory) with same name is found. Please try again with another name");
                break;

            case FILETOOBIG:
                System.out.println("The file is too big to be placed in this disk. Please try again after deleting some files or open a new disk");
                break;

            case NEWDOC:
                System.out.println("A new document " + docName + "." + fileType + " is created.");
                break;
        }
    }

    /**
     * feedbackCreateDirectory outputs feedback according to the situation of directory creation
     * @param status status of directory creation
     * @param dirName name of the directory to be created
     */
    public void feedbackCreateDirectory (Status status, String dirName) {
        switch (status) {
            case NODISK:
                System.out.println("No disk is being operated. Please create a disk");
                break;

            case HASSAMEFILE:
                System.out.println("A file (document/directory) with same name is found. Please try again with another name");
                break;

            case FILETOOBIG:
                System.out.println("The file is too big to be placed in this disk. Please try again after deleting some files or open a new disk");
                break;

            case NEWDIR:
                System.out.println("A new directory " + "\"" + dirName + "\"" + " is created.");
                break;
        }
    }

    /**
     * feedbackDeleteFile outputs feedback according to the situation of file deletion
     * @param status status of file deleetion
     * @param fileName name of the file to be deleted
     */
    public void feedbackDeleteFile (Status status, String fileName) {
        switch (status) {
            case NODISK:
                System.out.println("No disk is being operated. Please create a disk");
                break;

            case NOSUCHFILE:
                System.out.println("Cannot find " + "\"" + fileName + "\"");
                break;

            case DELETEFILE:
                System.out.println("\"" + fileName + "\"" + " is deleted");
                break;
        }
    }

    /**
     * feedbackRenameFile outputs feedback according to the situation of file rename
     * @param status status of file rename
     * @param oldName the original name of the file (the name to be changed)
     * @param newName the new name of the file
     */
    public void feedbackRenameFile (Status status, String oldName, String newName) {
        switch (status) {
            case NODISK:
                System.out.println("No disk is being operated. Please create a disk");
                break;

            case NOSUCHFILE:
                System.out.println("Cannot find " + "\"" + oldName + "\"");
                break;

            case HASSAMEFILE:
                System.out.println("A file (document/directory) with same name is found. Please try again with another name");
                break;

            case RENAMEFILE:
                System.out.println("File (document/directory) name is changed from " + "\"" +  oldName + "\"" + " to " + "\"" + newName + "\"");
                break;
        }
    }

    /**
     * feedbackChangeDirectory outputs feedback according to the situation of directory change
     * @param status status of the directory change
     * @param dirName name of the directory to change to
     */
    public void feedbackChangeDirectory (Status status, String dirName) {
        switch (status) {
            case NODISK:
                System.out.println("No disk is being operated. Please create a disk");
                break;

            case NOSUCHFILE:
                System.out.println("\"" + dirName + "\"" + " does not exists");
                break;

            case GOPARENT:
                System.out.println("Travelled to the parent directory");
                break;

            case NOPARENT:
                System.out.println("No parent directory to change to");
                break;

            case CHANGEDIR:
                System.out.println("Current directory is changed to " + "\"" + dirName + "\"");
                break;

            case NOTDIR:
                System.out.println("\"" + dirName + "\"" + "is not a directory");
                break;

        }
    }

    /**
     * feedbackNewSimpleCri outputs feedback according to the situation of simple criterion creation
     * @param status status of criterion creation
     * @param criName name of the criterion created
     */
    public void feedbackNewSimpleCri (Status status, String criName) {
        switch (status) {
            case HASSAMECRI:
                System.out.println("\"" + criName + "\"" + " already exists as a criterion");
                break;

            case NEWCRI:
                System.out.println("\"" + criName + "\"" + " is created as a criterion");
                break;
        }
    }

    /**
     * feedbackNewNegation outputs feedback according to the situation of the negation of a criterion
     * @param status status of the negation of a criterion
     * @param retName the name of the negated criteria
     * @param storeName the name of the original criteria
     */
    public void feedbackNewNegation (Status status, String retName, String storeName) {
        switch (status) {
            case NOSUCHCRI:
                System.out.println("Criterion " + "\"" + storeName + "\"" + " is not found");
                break;

            case HASSAMECRI:
                System.out.println("\"" + retName + "\"" + " is already a criterion");
                break;

            case NEWNEGATION:
                System.out.println("Negation of criterion " + "\"" + storeName + "\"" + " is stored as " + "\"" + retName + "\"");
                break;
        }
    }

    /**
     * @param status status of the situation of creation of binary criterion
     * @param retName name of the composite criterion created
     * @param storeName1 name of the first existing criterion
     * @param storeName2 name of the second exsiting criterion
     */
    public void feedbackNewBinaryCri (Status status, String retName, String storeName1, String storeName2) {
        switch (status) {
            case NOSUCHCRI:
                System.out.println("Criterion " + "\"" + storeName1 + "\" or " + "\"" + storeName2 + "\"" + " is not found");
                break;

            case HASSAMECRI:
                System.out.println("\"" + retName + "\"" + " is already a criterion");
                break;

            case NEWBINARYCRI:
                System.out.println("Composite criterion " + "\"" + retName + "\"" + " is created from criteria " + "\"" + storeName1 + "\" and " + "\"" + storeName2 + "\"");
                break;
        }
    }
    /**
     * feedbackQuit outputs a message when the user is quitting the current CVFS session
     */
    public void feedbackQuit () {
        System.out.println("You have quitted the Comp Virtual File System");
    }


    /**
     * REQ7 - List the files DIRECTLY contained in the current directory
     *        The total number of file and size will be reported in the end
     */
    public void listCurrentDirectory () {
        if (cvfs.getCurrentDisk() != null) {
            int sizeSum = 0;
            for (CompFile compFile : cvfs.getCurrentDirectory().getSubFile()) {
                sizeSum += compFile.getSize();
                if (compFile == cvfs.getCurrentDirectory().getSubFile().get(cvfs.getCurrentDirectory().getSubFile().size() - 1)) {
                    System.out.println(" └── " + compFile);
                } else {
                    System.out.println(" ├── " + compFile);
                }
            }
            System.out.println("Total number of file(s): " + cvfs.getCurrentDirectory().getSubFile().size());
            System.out.println("Total size: " + sizeSum);
        }
        else {
            System.out.println("No disk is being operated. Please create a disk");
        }
    }

    /**
     * REQ8 - List all the files in the current directory recursively, which includes files that are indirectly contained
     *        The total number of file and size will be reported in the end
     *        Since this requires recursion and Java does not allow multiple return values,
     *        this method is composed of different methods to print the total number and size of files properly
     *        The following is a wrapper method
     */
    public void listRecursiveDirectory () {
        //System.out.println(cvfs.getCurrentPath());
        if (cvfs.getCurrentDisk() != null) {
            // to the recursive method to recursively print the files
            if (cvfs.getCurrentDirectory().getSubFile() != null) {
                listRecursiveDirectory(cvfs.getCurrentDirectory(), "");
            }

            // count the total size with the method in class Directory
            int sizeSum = 0;
            for (CompFile compFile : cvfs.getCurrentDirectory().getSubFile()) {
                sizeSum += compFile.getSize();
            }

            // use method countRecursiveDirectory to get the total number of files indirectly contains
            int numFile = countRecursiveDirectory(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory());

            System.out.println("Total number of file(s): " + numFile);
            System.out.println("Total size: " + sizeSum);
        }
        else {
            // cvfs.getCurrentDisk () == null
            System.out.println("No disk is being operated. Please create a disk");
        }
    }

    /**
     * @param compFile the current directory and the directory inside the current directory
     * @param parentFile the current directory
     * @return the total number of file
     */
    public int countRecursiveDirectory (CompFile compFile, CompFile parentFile) {
        int tempSum = 0;
        if (compFile != parentFile) {
            tempSum++;
        }
        for (CompFile loopFile : compFile.getSubFile()) {
            if (loopFile.getFileType() == FileType.DOCUMENT) {
                tempSum += 1;
            }
            if (loopFile.getFileType() == FileType.DIRECTORY) {
                tempSum += countRecursiveDirectory(loopFile, parentFile);
            }
        }
        return tempSum;
    }

    /**
     * REQ8 - List all the files in the current directory recursively, which includes files that are indirectly contained
     * @param compFile the current file being accessed
     * @param indentBefore the string that will be printed before the file details
     */
    public void listRecursiveDirectory (CompFile compFile,  String indentBefore) {
        if (compFile.getSubFile() != null) {
            for (CompFile loopFile : compFile.getSubFile()) {
                if (loopFile != compFile.getSubFile().get(compFile.getSubFile().size() - 1)) {
                    System.out.println(indentBefore + " ├── " + loopFile);
                    listRecursiveDirectory(loopFile, indentBefore + " |      ");
                } else {
                    System.out.println(indentBefore + " └── " + loopFile);
                    listRecursiveDirectory(loopFile, indentBefore + "        ");
                }
            }
        }
    }

    /**
     * REQ12 printAllCriteria prints all the existing criteria in the CVFS
     * @param criteriaMap the criteria hash map from the CVFS model
     */
    public void printAllCriteria (CriteriaMap criteriaMap) {
        for (Object criName : criteriaMap.getHashMap().keySet()) {
            System.out.println(criName + " " + criteriaMap.getCriteria((String) criName));
        }
    }

    /**
     * REQ13 - searchFile finds the all files in the current directory that satisfies the given requirement
     * @param criName the criterion used to search files
     */
    public void searchFile (String criName) {
        int sizeSum = 0;
        int numFile = 0;
        if (cvfs.getCurrentDisk() != null) {
            if (cvfs.getCriteriaMap().hasSameCriteria(criName)) {
                for (CompFile compFile : cvfs.getCurrentDirectory().getSubFile()) {
                    // If it is the file that suit the criteria
                    if (cvfs.getCriteriaMap().getCriteria(criName).isCorrespondCondition(compFile)) {
                        sizeSum += compFile.getSize();
                        numFile++;
                        System.out.println(" --- " + compFile);
                    }
                }
                System.out.println("Total number of file(s): " + numFile);
                System.out.println("Total size: " + sizeSum);
            } else {
                // !cvfs.getCriteriaMap().hasSameCriteria(criName)
                System.out.println("Criterion " + "\"" + criName + "\"" + " is not found");
            }
        } else {
            // cvfs.getCurrentDisk() == null
            System.out.println("No disk is being operated. Please create a disk");
        }
    }

    /**
     * REQ14 - searchFileRecursive searches a file recursively
     * this is only wrapper
     * @param criName criteria name
     */
    public void searchFileRecursive(String criName) {
        if (cvfs.getCurrentDisk() != null) {
            if (cvfs.getCriteriaMap().hasSameCriteria(criName)) {
                if (cvfs.getCurrentDirectory().getSubFile() != null) {
                    searchFileRecursive(cvfs.getCurrentDirectory(), "", cvfs.getCriteriaMap().getCriteria(criName));
                }
                int numFile = countRecursiveDirectoryCriteria(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory(), cvfs.getCriteriaMap().getCriteria(criName));
                int sizeSum = sizeRecursiveDirectoryCriteria(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory(), cvfs.getCriteriaMap().getCriteria(criName), false);
                System.out.println("Total number of file(s): " + numFile);
                System.out.println("Total size: " + sizeSum);
            }
            else {
                System.out.println("Criterion " + "\"" + criName + "\"" + " is not found");
            }
        } else {
            System.out.println("No disk is being operated. Please create a disk");
        }
    }

    /**
     * searchFileRecursive searches a file recursively
     * this is the main recursive part
     * @param compFile the current directory (in the recursion)
     * @param indentBefore the indentation before printing
     * @param criteria the criteria to be checked
     */
    public void searchFileRecursive (CompFile compFile, String indentBefore, Criteria criteria) {
        if (compFile.getSubFile() != null) {
            for (CompFile loopFile : compFile.getSubFile()) {
                if (criteria.isCorrespondCondition(loopFile)) {
                    System.out.println(indentBefore + " --- " + loopFile);
                }
                else {
                    if (downwardSearch(loopFile, criteria)) {
                        System.out.println(indentBefore + " --- " + loopFile);
                    }
                }
                searchFileRecursive(loopFile, indentBefore + "        ", criteria);
            }
        }
    }

    /**
     * downwardSearch searches downward to see if there is any subfiles that satisfy the criteria
     * @param compFile the current directroy
     * @param criteria the criteria
     * @return true if there is a subfile that satisfies the criteria
     */
    public boolean downwardSearch (CompFile compFile, Criteria criteria) {
        boolean retBool = false;
        if (compFile.getSubFile() != null) {
            for (CompFile loopFile : compFile.getSubFile()) {
                if (criteria.isCorrespondCondition(loopFile)) {
                    retBool = true;
                }
                retBool = retBool || downwardSearch(loopFile, criteria);
            }
        }
        return retBool;
    }

    /**
     * countRecursiveDirectoryCriteria counts recursively the number of the files that suit the criteria
     * @param compFile current directory (in the recursion)
     * @param parentFile the original current directory
     * @param criteria the criteria to compare to
     * @return the number of files that satisfies the criteria
     */
    public int countRecursiveDirectoryCriteria (CompFile compFile, CompFile parentFile, Criteria criteria) {
        int tempSum = 0;
        if (compFile != parentFile) {
            if (criteria.isCorrespondCondition(compFile)) {
                tempSum++;
            }
        }
        for (CompFile loopFile : compFile.getSubFile()) {
            if (loopFile.getFileType() == FileType.DOCUMENT) {
                if (criteria.isCorrespondCondition(loopFile)) {
                    tempSum++;
                }
            }
            if (loopFile.getFileType() == FileType.DIRECTORY) {
                tempSum += countRecursiveDirectoryCriteria(loopFile, parentFile, criteria);
            }
        }
        return tempSum;
    }

    /**
     * sizeRecursiveDirectoryCriteria counts recursively the total size of the files that suit the criteria
     * @param compFile current directory (in the recursion)
     * @param parentFile the original current directory
     * @param criteria the criteria to compare to
     * @param topper boolean for the "toppest" directory, to prevent overlapped size
     * @return the total size of files that satisfies the criteria
     */
    public int sizeRecursiveDirectoryCriteria (CompFile compFile, CompFile parentFile, Criteria criteria, boolean topper) {
        int tempSum = 0;
        if (compFile != parentFile) {
            if (criteria.isCorrespondCondition(compFile)) {
                if (!topper) {
                    tempSum += compFile.getSize();
                    topper = true;
                }
            }
        }
        for (CompFile loopFile : compFile.getSubFile()) {
            if (loopFile.getFileType() == FileType.DOCUMENT) {
                if (criteria.isCorrespondCondition(loopFile) && !topper) {
                    tempSum += loopFile.getSize();
                }
            }
            if (loopFile.getFileType() == FileType.DIRECTORY) {
                tempSum += sizeRecursiveDirectoryCriteria(loopFile, parentFile, criteria, topper);
            }
        }
        return tempSum;
    }


    /**
     * @param fileName file name
     */
    public void getContent (String fileName) {
        CompFile targetFile = cvfs.getSameCompFile(fileName);
        if (targetFile != null) {
            if (targetFile.getFileType() == FileType.DOCUMENT) {
                System.out.println("Content: " + ((Document) targetFile).getDocContent());
            }
            if (targetFile.getFileType() == FileType.DIRECTORY) {
                System.out.println("\"" + fileName + "\"" + " is a directory");
            }
        }
    }
    /* Error Messages */

    /**
     * errorInvalidKeyword gives an error message if the first word of the command is wrong
     * @param keyword the wrong keyword
     */
    public void errorInvalidKeyword (String keyword) {
        if (keyword.equals("")) {
            System.out.println("Please enter a statement");
        }
        else{
            System.out.println("Invalid command or files \"" + keyword + "\"");
        }
    }

    /**
     * errorInvalidParaCount gives an error message if the number of parameter is wrong
     * @param given the number of parameter the user gave
     * @param required the required number of parameter
     */
    public void errorInvalidParaCount (int given, int required) {
        if (given > required) {
            System.out.println("Too many arguments in the statement! Given: " + given + " Required: " + required);
        }
        else {
            System.out.println("Too few arguments in the statement! Given: " + given + " Required: " + required);
        }
    }

    /**
     * errorInvalidParaType gives an error message if the type of parameter is wrong
     */
    public void errorInvalidParaType  () {
        System.out.println("Argument format is incorrect. Please type help for more information");
    }
}
