package hk.edu.polyu.comp.comp2021.cvfs.model;

/**
 * Document is an extension of CompFile
 * Represents a document in the disk
 * @author Lammy
 */
public class Document extends CompFile {
    /**
     * BASESIZE is the "base" size of all document
     * content length * 2 will be added to BASESIZE to get the document size
     */
    private static final int BASESIZE = 40;
    private String docType;
    private String docContent;
    private int docSize;

    /**
     * Constructor of Document
     * @param docName document name
     * @param parentFile parent reference
     * @param docType type of document
     * @param docContent document content
     */
    public Document (String docName, CompFile parentFile, String docType, String docContent) {
        super(docName, FileType.DOCUMENT, parentFile, null);
        this.docType = docType;
        this.docContent = docContent;
        docSize = BASESIZE + docContent.length() * 2;
        parentFile.getSubFile().add(this);
    }
    @Override
    public int getSize () {
        return this.docSize;
    }

    public String toString () {
        return this.getFileType() + " " + this.getFileName() + "." + this.docType + " - Size: " + this.docSize;
    }

    /**
     * @return document type
     */
    public String getDocType () {
        return this.docType;
    }

    /**
     * @return document content
     */
    public String getDocContent () {return this.docContent;}



}
