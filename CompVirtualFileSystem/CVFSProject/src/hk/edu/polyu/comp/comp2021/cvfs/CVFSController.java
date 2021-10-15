package hk.edu.polyu.comp.comp2021.cvfs;


import hk.edu.polyu.comp.comp2021.cvfs.CVFSView;
import hk.edu.polyu.comp.comp2021.cvfs.model.CVFS;
import hk.edu.polyu.comp.comp2021.cvfs.model.Status;

import java.math.BigInteger;
import java.util.Scanner;

/**
 * CVFS Controller
 * @author Lammy
 * This controller part of the MVC pattern only handles the user input, and serves as a bridge between the model and the view
 * Error-checking on user input is implemented here
 */
public class CVFSController {
    private final CVFS cvfs;
    private final CVFSView cvfsView;

    /**
     * Constructor of CVFS Controller
     * When this class is created, it will ask for input until the user types "quit"
     * @param cvfs the CVFS instance (for manipulation of files)
     * @param cvfsView the CVFS view (for output in console)
     */
    public CVFSController (CVFS cvfs, CVFSView cvfsView) {
        this.cvfs = cvfs;
        this.cvfsView = cvfsView;
    }

    /**
     * commandLineInput reads the user input
     * And transfer the input to commandSwitch to determine its meaning
     * This method is started since the instantiation CVFS Controller
     * And ends until the user input "quit"
     */
    public void commandLineInput () {
        boolean continueInput = true;
        System.out.println("Welcome to Comp Virtual File System!");
        do {
            if (!cvfs.getCurrentPath().equals("")) {
                System.out.printf(cvfs.getCurrentPath() + ":");
            } else {
                System.out.printf(cvfs.getCurrentPath());
            }
            Scanner inputScan = new Scanner(System.in);
            if (inputScan.hasNextLine()) {
                String inputString = inputScan.nextLine();
                continueInput = commandSwitch(inputString);
            }
        } while (continueInput);
    }

    /**
     * commandSwitch receives the user input
     * And carries out error checking (in method commandErrorChecking)
     * And carries out corresponding action
     * @param inputString the string from user input
     * @return a boolean that determines whether the CVFS session stops
     */
    public boolean commandSwitch(String inputString) {
        Status status;
        // Checking the input error of user
        String[] checkedArray = commandErrorChecking(inputString);
        // Boolean for determining whether to provide input
        boolean continueInput = true;
        // If the array is checked (non-null), find the corresponding branch in switch
        // Default stands for document / directory access
        // Accessing a document will print its content
        // Accessing a directory will do nothing
        if (checkedArray != null) {
            switch (checkedArray[0]) {
                case "newDisk":
                    status = cvfs.createDisk(Integer.parseInt(checkedArray[1]));
                    cvfsView.feedbackCreateDisk(status, checkedArray[1]);
                    break;

                case "newDoc":
                    status = cvfs.createDocument(checkedArray[1], checkedArray[2], checkedArray[3]);
                    cvfsView.feedBackCreateDocument(status ,checkedArray[1], checkedArray[2].toLowerCase());
                    break;

                case "newDir":
                    status = cvfs.createDirectory(checkedArray[1]);
                    cvfsView.feedbackCreateDirectory(status, checkedArray[1]);
                    break;

                case "delete":
                    status = cvfs.deleteFile(checkedArray[1]);
                    cvfsView.feedbackDeleteFile(status, checkedArray[1]);
                    break;

                case "rename":
                    status = cvfs.renameFile(checkedArray[1], checkedArray[2]);
                    cvfsView.feedbackRenameFile(status, checkedArray[1], checkedArray[2]);
                    break;

                case "changeDir":
                    status = cvfs.changeDirectory(checkedArray[1]);
                    cvfsView.feedbackChangeDirectory(status, checkedArray[1]);
                    break;

                case "list":
                    cvfsView.listCurrentDirectory();
                    break;

                case "rList":
                    cvfsView.listRecursiveDirectory();
                    break;

                case "newSimpleCri":
                    status = cvfs.newSimpleCri(checkedArray[1], checkedArray[2], checkedArray[3], checkedArray[4]);
                    cvfsView.feedbackNewSimpleCri(status, checkedArray[1]);
                    break;

                case "newNegation":
                    status = cvfs.newNegation(checkedArray[1], checkedArray[2]);
                    cvfsView.feedbackNewNegation(status, checkedArray[1], checkedArray[2]);
                    break;

                case "newBinaryCri":
                    status = cvfs.newBinaryCri(checkedArray[1], checkedArray[2], checkedArray[3], checkedArray[4]);
                    cvfsView.feedbackNewBinaryCri(status, checkedArray[1], checkedArray[2], checkedArray[4]);
                    break;

                case "printAllCriteria":
                    cvfsView.printAllCriteria(cvfs.getCriteriaMap());
                    break;

                case "search":
                    cvfsView.searchFile(checkedArray[1]);
                    break;

                case "rSearch":
                    cvfsView.searchFileRecursive(checkedArray[1]);
                    break;
                case "store":
                    break;
                case "load":
                    break;
                case "undo":
                    break;
                case "redo":
                    break;

                case "quit":
                    cvfsView.feedbackQuit();
                    continueInput = false;
                    break;

                default:
                    cvfsView.getContent(checkedArray[0]);
                    break;
            }
        }
        return continueInput;
    }


    /**
     * commandErrorChecking checks the error of user input, including:
     * invalid keywords, invalid number of arguments, invalid argument types
     * @param inputString the raw user input, in string
     * @return a splited string in array, if it is valid OR
     *         a null reference, if it is invalid
     */
    public String[] commandErrorChecking (String inputString) {
        // Keywords of commands
        String[] commandKeyword = {"newDisk", "newDoc", "newDir", "delete", "rename", "changeDir", "list", "rList",
                "newSimpleCri", "newNegation", "newBinaryCri", "printAllCriteria",
                "search", "rSearch", "quit"};
        // Keywords for document types
        String[] docTypeKeyword = {"css", "txt", "java", "html"};
        // The corresponding number of parameters or each command keywords
        // For example, commandKeyword[1] = "newDoc", and commandPara[1] = 3
        // So for command newDoc, we need to check if it has exactly 3 parameters
        int[] commandPara = {1, 3, 1, 1, 2, 1, 0, 0, 4, 2, 4, 0, 1, 1, 0};
        // The variable for finding the index of a keyword in commandKeyword and docTypeKeyword
        int commandLoop;
        // Boolean to decide what is wrong with the input string
        boolean isValidKeyword = false;
        boolean isValidParaCount = false;
        boolean isValidParaType = false;

        // Error-checking for keywords and parameters
        // Splitting the array for checking
        String[] splitArray = inputString.trim().split(" ");

        // Check if the command is newDoc, since a document content can contain space
        // So we only cut it into 4 parameters and check if the first 3 parameters (excluding the content) are valid
        if (splitArray[0].equals("newDoc")) {
            splitArray = inputString.replaceFirst("^\\s*", "").split(" ", 4);
        }
        else {
            splitArray = inputString.trim().split(" ");
        }

        // Check if keyword and corresponding number of parameter is correct
        for (commandLoop = 0; commandLoop < commandKeyword.length; commandLoop++) {
            if (splitArray[0].equals(commandKeyword[commandLoop])) {
                // Keyword is valid
                isValidKeyword = true;
                if (commandPara[commandLoop] == (splitArray.length - 1)) {
                    // The number of parameter(s) is correct
                    isValidParaCount = true;
                }
                break;
            }
        }

        // For the following switch statement, it will check the argument type (number or type of characters received)
        // Since some of the command has same argument type, we use fallthrough (no break) to prevent duplicated actions
        BigInteger maxInt;
        BigInteger paraValue;

        if (isValidKeyword && isValidParaCount) {
            switch (splitArray[0]) {
                case "newDisk":
                    // Check whether the first parameter only contains digits
                    isValidParaType = isParaOnlyLetterOrNum("number", splitArray[1]);
                    // Check if size is too large, by using BigInteger
                    if (isValidParaType) {
                        maxInt = BigInteger.valueOf(Integer.MAX_VALUE);
                        paraValue = new BigInteger(splitArray[1]);
                        if (paraValue.compareTo(maxInt) > 0) {
                            isValidParaType = false;
                        }
                    }
                    break;

                case "newDoc":
                    // Check whether the 2nd (docType) only contains the keyword
                    for (String keyword : docTypeKeyword) {
                        if (splitArray[2].equals(keyword)) {
                            isValidParaType = true;
                            break;
                        }
                    }
                    if (isValidParaType) {
                        // Check whether the 1st parameter (docName) only contains digits or letters
                        // And has <= 10 characters
                        isValidParaType = isParaOnlyLetterOrNum("mixed", splitArray[1]) && splitArray[1].length() <= 10;
                    }
                    break;

                case "rename":
                    // Check whether the 1st parameter (oldName) and the 2nd parameter (newName) only contains digits or letters
                    // Check whether both parameter has <= 10 characters
                    isValidParaType = isParaOnlyLetterOrNum("mixed", splitArray[1])
                            && isParaOnlyLetterOrNum("mixed", splitArray[2])
                            && splitArray[1].length() <= 10
                            && splitArray[2].length() <= 10;
                    break;

                case "changeDir":
                    isValidParaType = (splitArray[1].equals("..") || isParaOnlyLetterOrNum("mixed", splitArray[1]) && splitArray[1].length() <= 10);
                    break;

                case "newDir":
                case "delete":
                    // Check whether the 1st parameter (fileName) only contains digits or letters
                    // And has <= 10 characters
                    isValidParaType = isParaOnlyLetterOrNum("mixed", splitArray[1]) && splitArray[1].length() <= 10;
                    break;

                case "list":
                case "rList":
                case "quit":
                case "printAllCriteria":
                    isValidParaType = true;
                    break;

                case "newSimpleCri":
                    // Check if 1st parameter (criName) only contains 2 letters
                    if (isParaOnlyLetterOrNum("letter", splitArray[1]) && splitArray[1].length() == 2) {
                        // Check if attrName == name, whether other parameters are correct
                        if (splitArray[2].equals("name") && splitArray[3].equals("contains") && splitArray[4].charAt(0) == '"' && splitArray[4].charAt(splitArray[4].length() - 1) == '"') {
                            isValidParaType = true;
                        }
                        // Check if attrName == type, whether other parameters are correct
                        if (splitArray[2].equals("type") && splitArray[3].equals("equals") && splitArray[4].charAt(0) == '"' && splitArray[4].charAt(splitArray[4].length() - 1) == '"') {
                            isValidParaType = true;
                        }
                        // Check if attrName == size, whether other parameters are correct
                        if (splitArray[2].equals("size") && ((splitArray[3].equals(">")) || (splitArray[3].equals("<"))
                                || (splitArray[3].equals(">=")) || (splitArray[3].equals("<="))
                                || (splitArray[3].equals("==")) || (splitArray[3].equals("!=")))
                                && (isParaOnlyLetterOrNum("number", splitArray[4]))) {
                            isValidParaType = true;
                        }
                        // Check if overflow
                        if ((splitArray[2].equals("size")) && (isParaOnlyLetterOrNum("number", splitArray[4]))) {
                                maxInt = BigInteger.valueOf(Integer.MAX_VALUE);
                                paraValue = new BigInteger(splitArray[4]);
                                if (paraValue.compareTo(maxInt) > 0) {
                                    isValidParaType = false;
                                }
                        }
                    }
                    break;

                case "newNegation":
                    // Check if 1st parameter (criName1) only contains 2 letters
                    // Check if 2nd parameter (criName2) only contains either 2 letters or "IsDocument"
                    isValidParaType = ((splitArray[1].length() == 2) && (isParaOnlyLetterOrNum("letter", splitArray[1]))
                                   && ((splitArray[2].length() == 2) && (isParaOnlyLetterOrNum("letter", splitArray[2]))) || (splitArray[2].equals("IsDocument")));
                    break;

                case "newBinaryCri":
                    // Check if 1st (criName1) only contains 2 letters
                    // Check if 2nd, 4th parameter (criName3, criName4) either contain 2 letters or "IsDocument"
                    // Check if 3rd parameter (logicOp) is either "&&" or "||"
                    isValidParaType = (splitArray[1].length() == 2) && (isParaOnlyLetterOrNum("letter", splitArray[1]))
                                   && (((splitArray[2].length() == 2) && (isParaOnlyLetterOrNum("letter", splitArray[2]))) || splitArray[2].equals("IsDocument"))
                                   && (((splitArray[4].length() == 2) && (isParaOnlyLetterOrNum("letter", splitArray[4]))) || splitArray[4].equals("IsDocument"))
                                   && ((splitArray[3].equals("||")) || (splitArray[3].equals("&&")));
                    break;

                case "search":
                case "rSearch":
                    // Check whether the 1st parameter (criName) only contains 2 letters
                    // And has <= 10 characters
                    isValidParaType = (isParaOnlyLetterOrNum("letter", splitArray[1]) && splitArray[1].length() == 2) || (splitArray[1].equals("IsDocument"));
                    break;
            }
        }

        // If the keyword is not valid, we still need to see if it is indicating a file
        // This section checks if the keyword is the name of a file
        // And set the booleans to true if it is true
        boolean isFile = false;
        if (!isValidKeyword) {
            if (cvfs.getSameCompFile(splitArray[0]) != null) {
                isFile = true;
                isValidKeyword = true;
                if (splitArray.length == 1) {
                    isValidParaCount = true;
                    isValidParaType = true;
                }
            }
        }

        // Using CVFSView to notify users of error
        // If keyword is incorrect
        if (!isValidKeyword) {
            cvfsView.errorInvalidKeyword(splitArray[0]);
        } else {
            // If keyword is ok but number of parameter is not ok
            if (!isValidParaCount && !isFile) {
                cvfsView.errorInvalidParaCount(splitArray.length - 1, commandPara[commandLoop]);
            } else {
                // Both keyword and number of parameter are ok, but each parameter is not ok
                if (!isValidParaType) {
                    cvfsView.errorInvalidParaType();
                } else {
                    // Everything is ok, return the split statement to commandSwitch
                    // System.out.println("So far so good!");
                    return splitArray;
                }
            }
        }
        // If one of the three booleans are false, return null instead
        return null;
    }

    /* Utility Methods */
    /**
     * isParaOnlyLetterOrNum checks if the given string has only English letters, or digits, or both
     * @param type type of checking, it can be number (contains only digits), letter (contains only English letters), or both
     * @param checkString the string to be checked
     * @return true or false according to the type and the string
     */
    public boolean isParaOnlyLetterOrNum (String type, String checkString) {
        switch (type) {
            case "number":
                for (int i = 0; i < checkString.length(); i++) {
                    if (!Character.isDigit(checkString.charAt(i))) {
                        return false;
                    }
                }
                break;

            case "letter":
                for (int i = 0; i < checkString.length(); i++) {
                    if (!((checkString.charAt(i) >= 'a' && checkString.charAt(i) <= 'z')
                       || (checkString.charAt(i) >= 'A' && checkString.charAt(i) <= 'Z'))) {
                        return false;
                    }
                }
                break;

            case "mixed":
                for (int i = 0; i < checkString.length(); i++) {
                    if ((!((checkString.charAt(i) >= 'a' && checkString.charAt(i) <= 'z')
                       || (checkString.charAt(i) >= 'A' && checkString.charAt(i) <= 'Z')))
                       && (!Character.isDigit(checkString.charAt(i)))) {
                        return false;
                        }
                }
                break;
        }
        return true;
    }
}
