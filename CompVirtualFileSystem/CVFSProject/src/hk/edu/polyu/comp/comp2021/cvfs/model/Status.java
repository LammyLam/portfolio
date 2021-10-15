package hk.edu.polyu.comp.comp2021.cvfs.model;

/**
 * Enum of different status
 * Refer to CVFSView for its usage
 */
public enum Status {
    /**
     * NEWDISK = new disk is created successfully, without previous disk creation
     */
    NEWDISK,

    /**
     * CLOSEANDNEWDISK = the current disk is closed, and a new disk is created successfully
     */
    CLOSEANDNEWDISK,

    /**
     * NODISK = there is no disk being operated in the current CVFS session
     */
    NODISK,

    /**
     * HASSAMEFILE = there is a file (document/directory) of same name
     */
    HASSAMEFILE,

    /**
     * FILETOOBIG = the file is too big to be stored in the current disk
     */
    FILETOOBIG,

    /**
     * NEWDOC = a new document is successfully created
     */
    NEWDOC,

    /**
     * NEWDIR = a new directory is successfully created
     */
    NEWDIR,

    /**
     * NOSUCHFILE = there is no file with name that is same as the given one
     */
    NOSUCHFILE,

    /**
     * DELETEFILE = a file is successfully deleted
     */
    DELETEFILE,

    /**
     * RENAMEFILE = the file is successfully renamed
     */
    RENAMEFILE,

    /**
     * CHANGEDIR = the current directory is successfully changed
     */
    CHANGEDIR,

    /**
     * NOPARENT = there is no parent directory to change to
     */
    NOPARENT,

    /**
     * GOPARENT = the current directory is changed to the parent directory
     */
    GOPARENT,

    /**
     * NOTDIR = the CompFile is not directory
     */
    NOTDIR,

    /**
     * NEWCRI = a new criterion is successfully created
     */
    NEWCRI,

    /**
     * HASSAMECRI = there is already a criterion with name same as the given one
     */
    HASSAMECRI,

    /**
     * NOSUCHCRI = there is no criterion with the same name
     */
    NOSUCHCRI,

    /**
     * NEWNEGATION = a new negated criterion is created
     */
    NEWNEGATION,

    /**
     * NEWBINARYCRI = a new binary composite criterion is created
     */
    NEWBINARYCRI
}
