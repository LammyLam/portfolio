package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

/**
 * Criteria is a list of String
 * It uses reverse Polish notation to obtain the expression needed for searching
 * @author Lammy
 */
public class Criteria {

    private List<String> criteriaComponentList;
    private List<String> infixList;
    private String criName;

    /**
     * Constructor of Criteria (if user asks for a simple criterion)
     * @param criName criterion name
     * @param attrName attribute name (e.g. name, type, size)
     * @param op operator (e.g. >=, contains, equals)
     * @param val a value for comparison
     */
    public Criteria (String criName, String attrName, String op, String val) {
        this.criName = criName;
        criteriaComponentList = new ArrayList<>();
        criteriaComponentList.add(attrName);
        criteriaComponentList.add(val);
        criteriaComponentList.add(op);

        infixList = new ArrayList<>();
        infixList.add("(");
        infixList.add(attrName);
        infixList.add(op);
        infixList.add(val);
        infixList.add(")");
    }

    /**
     * Constructor of Criteria (if user asks for an negated criterion)
     * @param criName criterion name
     * @param criteria the criterion to be negated
     */
    public Criteria (String criName, Criteria criteria) {
        this.criName = criName;
        criteriaComponentList = new ArrayList<>();
        this.criteriaComponentList.addAll(criteria.getCriteriaComponentList());
        criteriaComponentList.add("!");

        infixList = new ArrayList<>();
        infixList.add("(");
        infixList.add("!");
        this.infixList.addAll(criteria.getInfixList());
        infixList.add(")");
    }

    /**
     * Constructor of IsDocument Criteria
     * Automatically assign the criteria as (FileType equals Document)
     */
    public Criteria () {
        this.criName = "IsDocument";
        criteriaComponentList = new ArrayList<>();
        criteriaComponentList.add("FileType");
        criteriaComponentList.add("Document");
        criteriaComponentList.add("equals");

        infixList = new ArrayList<>();
        infixList.add("(");
        infixList.add("FileType");
        infixList.add("equals");
        infixList.add("Document");
        infixList.add(")");
    }

    /**
     * Constructor of Criteria (if user asks for a composite criterion)
     * @param criName the name of the composite criterion to be created
     * @param criteria1 the first existing criteria
     * @param logicOp the logic operator
     * @param criteria2 the second existing criteria
     */
    public Criteria (String criName, Criteria criteria1, String logicOp, Criteria criteria2) {
        this.criName = criName;
        this.criteriaComponentList = new ArrayList<>();
        this.criteriaComponentList.addAll(criteria1.getCriteriaComponentList());
        this.criteriaComponentList.addAll(criteria2.getCriteriaComponentList());
        this.criteriaComponentList.add(logicOp);

        infixList = new ArrayList<>();
        infixList.add("(");
        this.infixList.addAll(criteria1.getInfixList());
        this.infixList.add(logicOp);
        this.infixList.addAll(criteria2.getInfixList());
        infixList.add(")");
    }

    /**
     * isCorrespondCondition finds whether a file satisfies the condition of this criteria
     * By using a list and criteria's reverse polish notation
     * @param compFile the file to checked
     * @return true or false depending on whether file satisfies the condition of this criteria
     */
    public boolean isCorrespondCondition (CompFile compFile) {
        Stack <String> polishStack = new Stack<>();
        for (String criteriaComponent : criteriaComponentList) {
            // Check for logicOp, if there logicOp, "clean" the stack immediately
            polishStack.push(criteriaComponent);
            //System.out.println(polishStack);
            if (criteriaComponent.equals("!") || criteriaComponent.equals("&&") || criteriaComponent.equals("||")
                    || criteriaComponent.equals(">") || criteriaComponent.equals("<") || criteriaComponent.equals(">=")
                    || criteriaComponent.equals("<=") || criteriaComponent.equals("==") || criteriaComponent.equals("!=")
                    || criteriaComponent.equals("contains") || criteriaComponent.equals("equals")) {
                    if (polishStack.size() >= 3) {
                        polishStack.push(String.valueOf(evaluate(polishStack.pop(), polishStack.pop(), polishStack.pop(), compFile)));
                    }
                    else {
                        if (polishStack.size() == 2) {
                            polishStack.push(String.valueOf(evaluate(polishStack.pop(), polishStack.pop())));
                        }
                    }
                }
            //System.out.println(polishStack);
        }
        //System.out.println(Boolean.valueOf(polishStack.peek()));
        return Boolean.parseBoolean(polishStack.pop());
    }

    /**
     * This evaluate is a special version of the original evaluate(with 4 parameters)
     * It is used only for when evaluating a negated simple criteria (i.e. (!(name contains "haha"))
     * @param logicOp logicOp (must be "!")
     * @param val boolean value
     * @return true if the val is false and false if the val is true
     */
    public boolean evaluate (String logicOp, String val) {
        if (logicOp.equals("!")) {
            return !val.equals("true");
        }
        return false;
    }

    /**
     * Evaluate the result of comparison between compFile and the criteria
     * @param logicOp logical operator
     * @param val value to be compared OR a boolean value
     * @param attrName attribute to be compared OR a boolean value
     * @param compFile the compFile to be compared
     * @return true or false considering whether compFile satisfies the criteria
     */
    public boolean evaluate (String logicOp, String val, String attrName, CompFile compFile) {
        //System.out.println("Eval" + logicOp + val + attrName);
        if ((attrName.equals("true") || attrName.equals("false")) && (val.equals("true") || val.equals("false"))) {
            if (logicOp.equals("&&")) {
                //System.out.println(Boolean.valueOf(attrName) && Boolean.valueOf(val));
                return Boolean.parseBoolean(attrName) && Boolean.parseBoolean(val);
            }
            else {
                return Boolean.parseBoolean(attrName) || Boolean.parseBoolean(val);
            }
        }

        else {
            switch (attrName) {
                case "name":
                    if (logicOp.equals("contains")) {
                        return compFile.getFileName().contains(val.substring(1, val.length() - 1));
                    }

                case "type":
                    if (compFile.getFileType() == FileType.DOCUMENT) {
                        if (logicOp.equals("equals")) {
                            return ((Document) compFile).getDocType().equals(val.substring(1, val.length() - 1));
                        }
                    }

                case "size":
                    switch (logicOp) {
                        case ">":
                            return compFile.getSize() > Integer.parseInt(val);

                        case "<":
                            return compFile.getSize() < Integer.parseInt(val);

                        case ">=":
                            return compFile.getSize() >= Integer.parseInt(val);

                        case "<=":
                            return compFile.getSize() <= Integer.parseInt(val);

                        case "==":
                            return compFile.getSize() == Integer.parseInt(val);

                        case "!=":
                            return compFile.getSize() != Integer.parseInt(val);
                    }

                case "FileType":
                    if (logicOp.equals("equals")) {
                        //System.out.println("ha" + (compFile.getFileType() == FileType.DOCUMENT));
                        return compFile.getFileType() == FileType.DOCUMENT;
                    }
            }
        }
        return false;
    }

    public String toString () {
        StringBuilder sB = new StringBuilder();
        for (String criteriaComponent : infixList) {
            sB.append(criteriaComponent).append(" ");
        }
        return sB.toString();
    }

    /**
     * @return this criterion's component list (in reverse polish notation)
     */
    public List<String> getCriteriaComponentList () {
        return this.criteriaComponentList;
    }

    /**
     * @return this criterion's component list (in infix notation)
     */
    public List<String> getInfixList () {
        return this.infixList;
    }



}
