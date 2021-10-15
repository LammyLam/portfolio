package hk.edu.polyu.comp.comp2021.cvfs.model;



import hk.edu.polyu.comp.comp2021.cvfs.CVFSController;
import hk.edu.polyu.comp.comp2021.cvfs.CVFSView;
import org.junit.Before;
import org.junit.Test;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.Arrays;
import java.util.List;

import static org.junit.jupiter.api.Assertions.*;

/**
 * CVFSTests is for coverage tests, by hard-coding the input to simulate real-time operation
 */
public class CVFSTest {

    private CVFS cvfs;
    private CVFSView cvfsView;
    private CVFSController cvfsController;

    /**
     * Preparation before actually testing the unit
     */
    @Before
    public void CVFSPreparation() {
        cvfs = new CVFS();
        cvfsView = new CVFSView(cvfs);
        cvfsController = new CVFSController(cvfs, cvfsView);
    }

    /* Section - Input Error */
    /* The input is wrong if returns null */

    /**
     * newDisk input error
     */

    @Test
    public void newDiskInputError() {
        String[] correctStatement = {"newDisk", "1024"};
        assertNull(cvfsController.commandErrorChecking("NEWDISK"));
        assertNull(cvfsController.commandErrorChecking("newDisk haha"));
        assertNull(cvfsController.commandErrorChecking("newDisk aa6"));
        assertNull(cvfsController.commandErrorChecking("newDisk 100000000000000000000000000000000"));
        assertNull(cvfsController.commandErrorChecking("newDisk"));
        assertArrayEquals(cvfsController.commandErrorChecking("newDisk 1024"), correctStatement);
    }

    /**
     * newDoc input error
     */
    @Test
    public void newDocInputError() {
        String[] correctStatement = {"newDoc", "fruitKing", "txt", "Fear me banana"};
        assertNull(cvfsController.commandErrorChecking("NEWDOC"));
        assertNull(cvfsController.commandErrorChecking("newDoc haha this very wrong"));
        assertNull(cvfsController.commandErrorChecking("newDoc kingOfFruitTooLong ttt Fear me banana"));
        assertNull(cvfsController.commandErrorChecking("newDoc fruitKing"));
        assertArrayEquals(cvfsController.commandErrorChecking("newDoc fruitKing txt Fear me banana"), correctStatement);

    }

    /**
     * newDir input error
     */
    @Test
    public void newDirInputError() {
        String[] correctStatement = {"newDir", "pineapple"};
        assertNull(cvfsController.commandErrorChecking("NEWDIR"));
        assertNull(cvfsController.commandErrorChecking("newDir 100000000000000000000000000000000"));
        assertNull(cvfsController.commandErrorChecking("newDir"));
        assertNull(cvfsController.commandErrorChecking("newDir haha this wrong"));
        assertArrayEquals(cvfsController.commandErrorChecking("newDir pineapple"), correctStatement);
    }

    /**
     * delete input error
     */
    @Test
    public void deleteInputError() {
        String[] correctStatement = {"delete", "orange"};
        assertNull(cvfsController.commandErrorChecking("delete too much I think"));
        assertNull(cvfsController.commandErrorChecking("delete"));
        assertNull(cvfsController.commandErrorChecking("DELETE"));
        assertNull(cvfsController.commandErrorChecking(""));
        assertArrayEquals(cvfsController.commandErrorChecking("delete orange"), correctStatement);
    }

    /**
     * rename input error
     */
    @Test
    public void renameInputError() {
        String[] correctStatement = {"rename", "pineapple", "apple"};
        assertNull(cvfsController.commandErrorChecking("reName"));
        assertNull(cvfsController.commandErrorChecking("rename aha wrong la"));
        assertNull(cvfsController.commandErrorChecking("rename lo"));
        assertArrayEquals(cvfsController.commandErrorChecking("rename pineapple apple"), correctStatement);
    }

    /**
     * changeDir input error
     */
    @Test
    public void changeDirInputError() {
        String[] correctStatement = {"changeDir", "peach"};
        assertNull(cvfsController.commandErrorChecking("moveDDDDir"));
        assertNull(cvfsController.commandErrorChecking("changeDir too much lo"));
        assertNull(cvfsController.commandErrorChecking("changeDir"));
        assertNull(cvfsController.commandErrorChecking("changeDir 試下中文先"));
        assertArrayEquals(cvfsController.commandErrorChecking("changeDir peach"), correctStatement);
    }

    /**
     * list input error
     */
    @Test
    public void listInputError() {
        String[] correctStatement = {"list"};
        assertNull(cvfsController.commandErrorChecking("list everything la"));
        assertNull(cvfsController.commandErrorChecking("LISTEVERYTHING"));
        assertArrayEquals(cvfsController.commandErrorChecking("list"), correctStatement);
    }

    /**
     * rList input error
     */
    @Test
    public void rListInputError() {
        String[] correctStatement = {"rList"};
        assertNull(cvfsController.commandErrorChecking("rLIST"));
        assertNull(cvfsController.commandErrorChecking("rList everything ah wei"));
        assertArrayEquals(cvfsController.commandErrorChecking("rList"), correctStatement);
    }

    /**
     * newSimpleCri input error
     */
    @Test
    public void newSimpleCriInputError() {
        String[] correctStatement1 = {"newSimpleCri", "aa", "name", "contains", "\"banana\""};
        String[] correctStatement2 = {"newSimpleCri", "aa", "type", "equals", "\"html\""};
        String[] correctStatement3 = {"newSimpleCri", "aa", "size", ">", "269"};
        assertNull(cvfsController.commandErrorChecking("newSimpleCry"));
        assertNull(cvfsController.commandErrorChecking("newSimpleCri aa > da"));
        assertNull(cvfsController.commandErrorChecking("newSimpleCri aa size >> 10"));
        assertNull(cvfsController.commandErrorChecking("newSimpleCri aa size >> 100000000000000000"));
        assertNull(cvfsController.commandErrorChecking("newSimpleCri aa type contains \"banana\""));
        assertNull(cvfsController.commandErrorChecking("newSimpleCri aa name contains banana"));
        assertArrayEquals(cvfsController.commandErrorChecking("newSimpleCri aa name contains \"banana\""), correctStatement1);
        assertArrayEquals(cvfsController.commandErrorChecking("newSimpleCri aa type equals \"html\""), correctStatement2);
        assertArrayEquals(cvfsController.commandErrorChecking("newSimpleCri aa size > 269"), correctStatement3);
    }

    /**
     * newNegation input error
     */
    @Test
    public void newNegationInputError() {
        String[] correctStatement = {"newNegation", "aa", "bb"};
        assertNull(cvfsController.commandErrorChecking("NewNegation"));
        assertNull(cvfsController.commandErrorChecking("newNegation bb"));
        assertNull(cvfsController.commandErrorChecking("newNegation IsDocument bb"));
        assertArrayEquals(cvfsController.commandErrorChecking("newNegation aa bb"), correctStatement);
    }

    /**
     * newBinaryCri input error
     */
    @Test
    public void newBinaryCriInputError() {
        String[] correctStatement = {"newBinaryCri", "cc", "aa", "&&", "bb"};
        assertNull(cvfsController.commandErrorChecking("newBinaryCry"));
        assertNull(cvfsController.commandErrorChecking("newBinaryCri aa bb >> cc"));
        assertNull(cvfsController.commandErrorChecking("newBinaryCri aa bb && 12"));
        assertNull(cvfsController.commandErrorChecking("newBinaryCri aa cc || jkl"));
        assertArrayEquals(cvfsController.commandErrorChecking("newBinaryCri cc aa && bb"), correctStatement);
    }

    /**
     * rSearch input error
     */
    @Test
    public void rSearchInputError() {
        String[] correctStatement = {"rSearch", "aa"};
        assertNull(cvfsController.commandErrorChecking("rSearch"));
        assertNull(cvfsController.commandErrorChecking("rSearch jkl"));
        assertNull(cvfsController.commandErrorChecking("rSearch 12"));
        assertArrayEquals(cvfsController.commandErrorChecking("rSearch aa"), correctStatement);
    }

    /* Logic Test */

    /**
     * newDisk logic test
     */
    @Test
    public void newDiskLogicTest() {
        final int correctDiskSize = 1024;
        // Check if a disk exists
        assertNull(cvfs.getCurrentDisk());
        // Create a disk with size 1024 bytes
        cvfs.createDisk(correctDiskSize);
        // See if there is a disk
        assertNotNull(cvfs.getCurrentDisk());
        // See if the disk has a capacity of 1024 bytes
        assertEquals(cvfs.getCurrentDisk().getCapacity(), correctDiskSize);
    }

    /**
     * newDoc logic test
     */
    @Test
    public void newDocLogicTest() {
        final String correctString = "exists";
        final int bigDiskSize = 1024;
        final int smallDiskSize = 10;
        // Create a document without operating a disk
        cvfs.createDocument("haha", "txt", "weewaahoo");
        // Check if the document exists
        assertNull(cvfs.getSameCompFile("haha"));
        // Create a disk
        cvfs.createDisk(bigDiskSize);
        // Create a document with a disk operating
        cvfs.createDocument("haha", "txt", "weewaahoo");
        // Check if a disk contains the file
        assertNotNull(cvfs.getSameCompFile("haha"));
        // Create a disk with size 10 bytes (not enough to hold a file)
        cvfs.createDisk(smallDiskSize);
        // Create a document
        cvfs.createDocument("haha", "txt", "weewaahoo");
        // See if a document can still be created with the lack of space
        assertNull(cvfs.getSameCompFile("haha"));
        // Create a disk
        cvfs.createDisk(bigDiskSize);
        // Create a document
        cvfs.createDocument("haha", "txt", "exists");
        // Create a document with the same name
        cvfs.createDocument("haha", "txt", "no la");
        // See if a document's content to determine if file name can be repeated
        assertEquals(((Document) cvfs.getSameCompFile("haha")).getDocContent(), correctString);
    }

    /**
     * newDir logic test
     */
    @Test
    public void newDirLogicTest() {
        final int bigDiskSize = 1024;
        final int smallDiskSize = 10;
        final int correctDirectoryNumber = 1;
        // Create a directory without a disk operating
        cvfs.createDirectory("banana");
        // See if the directory exists
        assertNull(cvfs.getSameCompFile("banana"));
        // Create a disk
        cvfs.createDisk(bigDiskSize);
        // Create a directory with a disk operating
        cvfs.createDirectory("banana");
        assertNotNull(cvfs.getSameCompFile("banana"));
        // Create another directory with the same name
        cvfs.createDirectory("banana");
        // Check if there is only one directory
        assertEquals(cvfsView.countRecursiveDirectory(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory()), correctDirectoryNumber);
        // Create a disk with size 10 bytes
        cvfs.createDisk(smallDiskSize);
        // Create a directory (disk is not able to hold it)
        cvfs.createDirectory("banana");
        // Check if the directory exists
        assertNull(cvfs.getSameCompFile("banana"));
    }

    /**
     * delete logic test
     */
    @Test
    public void deleteLogicTest() {
        final int bigDiskSize = 1024;
        // Delete a file when there is not even a disk, check if the status is NODISK
        assertEquals(cvfs.deleteFile("banana"), Status.NODISK);
        // Create a disk
        cvfs.createDisk(bigDiskSize);
        // Delete a file when there is no such file, check if the status is NOSUCHFILE
        assertEquals(cvfs.deleteFile("banana"), Status.NOSUCHFILE);
        // Create a directory
        cvfs.createDirectory("banana");
        // Check if that directory exists
        assertNotNull(cvfs.getSameCompFile("banana"));
        // Delete the directory
        cvfs.deleteFile("banana");
        // Check if that directory still exists
        assertNull(cvfs.getSameCompFile("banana"));
    }

    /**
     * rename logic test
     */
    @Test
    public void renameLogicTest() {
        final int bigDiskSize = 1024;
        // rename a file when there is not even a disk and see if it returns NODISK
        assertEquals(cvfs.renameFile("haha", "crycry"), Status.NODISK);
        // create a disk with size 1024
        cvfs.createDisk(bigDiskSize);
        // rename a file when there is not even a file
        assertEquals(cvfs.renameFile("haha", "crycry"), Status.NOSUCHFILE);
        // Create a directory
        cvfs.createDirectory("big lam");
        // rename the directory
        cvfs.renameFile("big lam", "small lam");
        // Check if the file with the old name exists
        assertNull(cvfs.getSameCompFile("big lam"));
        // Check if the file with the new name exists
        assertNotNull(cvfs.getSameCompFile("small lam"));
        // Create a document
        cvfs.createDocument("middle lam", "txt", "hahahahhaa");
        // rename a file to a name that already exists, see if it returns HASSAMEFILE
        assertEquals(cvfs.renameFile("small lam", "middle lam"), Status.HASSAMEFILE);
    }

    /**
     * changeDir logic test
     */
    @Test
    public void changeDirLogicTest() {
        final int bigDiskSize = 1024;
        // Change directory when there is not even a disk
        cvfs.changeDirectory("banana");
        // See if the current directory is null
        assertNull(cvfs.getCurrentDirectory());
        // Create a disk
        cvfs.createDisk(bigDiskSize);
        // Change directory when there is not even a directory
        cvfs.changeDirectory("banana");
        // See if the current directory is a non-existing one
        assertNotEquals(cvfs.getCurrentDirectory().getFileName(), "banana");
        // Create a directory
        cvfs.createDirectory("banana");
        // Change to that newly created directory
        cvfs.changeDirectory("banana");
        assertEquals(cvfs.getCurrentPath(), "Disk0\\banana");
        // See if the current directory is the newly created directory
        assertEquals(cvfs.getCurrentDirectory().getFileName(), "banana");
        // Change the directory to the parent file
        cvfs.changeDirectory("..");
        // See if the current directory is disk
        assertEquals(cvfs.getCurrentDirectory(), cvfs.getCurrentDisk());
        // Change the directory to the parent file (does not exist)
        assertEquals(cvfs.changeDirectory(".."), Status.NOPARENT);
    }

    /**
     * list logic test
     */

    @Test
    public void listLogicTest() {
        final int bigDiskSize = 1024;
        final int correctSize = 200;
        // List the current directory
        cvfsView.listCurrentDirectory();
        cvfs.createDisk(bigDiskSize);
        cvfs.createDirectory("banana");
        cvfs.createDirectory("apple");
        cvfs.createDirectory("hae");
        cvfs.changeDirectory("hae");
        cvfs.createDirectory("bla");
        cvfs.createDirectory("boa");
        cvfs.changeDirectory("..");
    }

    /**
     * rList logic test
     */

    @Test
    public void rListLogicTest() {
        final int bigDiskSize = 1024;
        final int correctNum = 5;
        // List the current directory
        cvfsView.listRecursiveDirectory();
        cvfs.createDisk(bigDiskSize);
        cvfs.createDirectory("banana");
        cvfs.createDirectory("apple");
        cvfs.createDirectory("hae");
        cvfs.changeDirectory("hae");
        cvfs.createDirectory("bla");
        cvfs.createDirectory("boa");
        cvfs.changeDirectory("..");
        assertEquals(cvfsView.countRecursiveDirectory(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory()), correctNum);
    }

    /**
     * newSimpleCri logic test
     */
    @Test
    public void newSimpleCriLogicTest() {
        // Create new criteria
        assertEquals(cvfs.newSimpleCri("aa", "size", ">", "40"), Status.NEWCRI);
        assertTrue(cvfs.getCriteriaMap().getHashMap().containsKey("aa"));
        // Create new criteria with the same name as the previous crieteria
        assertEquals(cvfs.newSimpleCri("aa", "size", ">", "40"), Status.HASSAMECRI);

        // Testing evaluation of criteria
        final int bigDiskSize = 1024;
        cvfs.createDisk(bigDiskSize);
        cvfs.createDirectory("banana");
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("!", "true"), false);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("!", "false"), true);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("true","false", "&&", cvfs.getSameCompFile("banana")), false);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("equals","\"css\"", "type", cvfs.getSameCompFile("banana")), false);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate(">","10", "size", cvfs.getSameCompFile("banana")), true);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate(">=","10", "size", cvfs.getSameCompFile("banana")), true);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("==","40", "size", cvfs.getSameCompFile("banana")), true);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("<","50", "size", cvfs.getSameCompFile("banana")), true);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("<=","10", "size", cvfs.getSameCompFile("banana")), false);
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").evaluate("equals","Document", "FileType", cvfs.getSameCompFile("banana")), false);
    }

    /**
     * logic test
     */
    @Test
    public void newNegationLogicTest() {
        // Check creation of criteria
        final List<String> correctStringList1 = Arrays.asList("size", "40", ">");
        cvfs.newSimpleCri("aa", "size", ">", "40");
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").getCriteriaComponentList(), correctStringList1);

        // Check negation of a crtierion
        final List<String> correctStringList2 = Arrays.asList("size", "40", ">", "!");
        cvfs.newNegation("bb", "aa");
        assertEquals(cvfs.getCriteriaMap().getCriteria("bb").getCriteriaComponentList(), correctStringList2);

        // Check negation of negation
        final List<String> correctStringList3 = Arrays.asList("size", "40", ">", "!", "!");
        cvfs.newNegation("cc", "bb");
        assertEquals(cvfs.getCriteriaMap().getCriteria("cc").getCriteriaComponentList(), correctStringList3);

        // Check negation of IsDocument
        final List<String> correctStringList4 = Arrays.asList("FileType", "Document", "equals", "!");
        cvfs.newNegation("dd", "IsDocument");
        assertEquals(cvfs.getCriteriaMap().getCriteria("dd").getCriteriaComponentList(), correctStringList4);
    }

    /**
     * newBinaryCri logic test
     */

    @Test
    public void newBinaryCriLogicTest() {
        // Check creation of criteria
        assertEquals(cvfs.newBinaryCri("cc", "bb", "&&", "aa"), Status.NOSUCHCRI);

        final List<String> correctStringList1 = Arrays.asList("size", "40", ">");
        cvfs.newSimpleCri("aa", "size", ">", "40");
        assertEquals(cvfs.getCriteriaMap().getCriteria("aa").getCriteriaComponentList(), correctStringList1);

        // Check negation of a crtierion
        final List<String> correctStringList2 = Arrays.asList("size", "40", ">", "!");
        cvfs.newNegation("bb", "aa");
        assertEquals(cvfs.getCriteriaMap().getCriteria("bb").getCriteriaComponentList(), correctStringList2);

        // See if a composite criteria can be created with the same name
        assertEquals(cvfs.newBinaryCri("aa", "bb", "&&", "aa"), Status.HASSAMECRI);

        // Check creation of composite criteria
        final List<String> correctStringList3 = Arrays.asList("size", "40", ">", "!", "size", "40", ">", "&&");

        cvfs.newBinaryCri("cc", "bb", "&&", "aa");
        assertEquals(cvfs.getCriteriaMap().getCriteria("cc").getCriteriaComponentList(), correctStringList3);

        // Check creation of composite criteria from a composite criterion and a simple criterion
        final List<String> correctStringList4 = Arrays.asList("size", "40", ">", "!", "size", "40", ">", "&&", "size", "40", ">", "&&");
        cvfs.newBinaryCri("dd", "cc", "&&", "aa");
        assertEquals(cvfs.getCriteriaMap().getCriteria("dd").getCriteriaComponentList(), correctStringList4);

        // Check creation of composite criteria from two composite criteria
        final List<String> correctStringList5 = Arrays.asList("size", "40", ">", "!", "size", "40", ">", "&&", "size", "40", ">", "!", "size", "40", ">", "&&", "&&");
        cvfs.newBinaryCri("ee", "cc", "&&", "cc");
        assertEquals(cvfs.getCriteriaMap().getCriteria("ee").getCriteriaComponentList(), correctStringList5);
    }

    /**
     * printAllCriteria logic test
     */
    @Test
    public void printAllCriteriaLogicTest() {
        cvfs.newSimpleCri("aa", "size", ">", "40");
        cvfs.newNegation("bb", "aa");
        cvfs.newBinaryCri("cc", "bb", "&&", "aa");
        cvfs.newBinaryCri("dd", "cc", "&&", "aa");
        cvfsView.printAllCriteria(cvfs.getCriteriaMap());
    }

    /**
     * search logic test
     */
    @Test
    public void searchLogicTest() {
        final int bigDiskSize = 1024;
        cvfs.newSimpleCri("aa", "size", "==", "40");
        cvfs.newSimpleCri("bb", "name", "contains", "\"wahaha\"");
        cvfs.newNegation("bb", "aa");
        cvfs.createDisk(bigDiskSize);
        cvfs.createDirectory("banana");
        cvfs.createDocument("wahaha", "txt", "water");
        cvfs.createDocument("waahahahaha", "txt", "water");
        cvfs.newBinaryCri("zz", "aa", "||", "bb");
        assertTrue(cvfs.getCriteriaMap().getCriteria("zz").isCorrespondCondition(cvfs.getSameCompFile("banana")));
        assertTrue(cvfs.getCriteriaMap().getCriteria("zz").isCorrespondCondition(cvfs.getSameCompFile("wahaha")));
        assertFalse(cvfs.getCriteriaMap().getCriteria("zz").isCorrespondCondition(cvfs.getSameCompFile("waahahahaha")));
    }

    /**
     * rSearch logic test
     */
    @Test
    public void rSearchLogicTest() {
        final int bigDiskSize = 1024;
        cvfs.newNegation("aa", "IsDocument");
        cvfs.newSimpleCri("bb", "size", "==", "40");
        cvfs.newBinaryCri("cc", "aa", "&&", "bb");
        cvfs.newSimpleCri("ee", "size", "!=", "40");
        cvfs.newSimpleCri("ee", "size", "!=", "40");
        cvfs.newSimpleCri("ff", "size", "<=", "40");
        cvfs.newSimpleCri("gg", "size", ">=", "40");
        cvfs.newSimpleCri("hh", "size", "<", "40");
        cvfs.newSimpleCri("ii", "size", ">", "40");
        cvfs.newSimpleCri("jj", "type", "equals", "\"txt\"");
        cvfs.createDisk(bigDiskSize);
        cvfs.createDirectory("banana");
        cvfs.changeDirectory("banana");
        cvfs.createDocument("wahaha", "txt", "water");
        cvfs.createDirectory("oil");
        cvfs.changeDirectory("oil");
        cvfs.createDirectory("salt");
        cvfs.changeDirectory("..");
        cvfs.changeDirectory("..");
        cvfs.createDirectory("apple");
        cvfs.changeDirectory("apple");
        cvfs.createDirectory("orange");
        cvfs.changeDirectory("..");
        cvfsController.commandSwitch("rList");
        final int correctNumber1 = 2;
        final int correctNumber2 = 4;
        final int correctSize1 = 80;
        final int correctSize2 = 90;
        assertEquals(cvfsView.countRecursiveDirectoryCriteria(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory(), cvfs.getCriteriaMap().getCriteria("bb")), correctNumber1);
        assertEquals(cvfsView.sizeRecursiveDirectoryCriteria(cvfs.getCurrentDirectory(), cvfs.getCurrentDirectory(), cvfs.getCriteriaMap().getCriteria("bb"), false), correctSize1);
    }
}