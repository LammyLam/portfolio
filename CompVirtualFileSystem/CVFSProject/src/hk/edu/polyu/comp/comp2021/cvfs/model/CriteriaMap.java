package hk.edu.polyu.comp.comp2021.cvfs.model;

import java.util.HashMap;

/**
 * CrteriaMap stores different criteria in a CVFS
 * by using HashMap
 * @author Lammy
 */
public class CriteriaMap {
    private HashMap<String, Criteria> criteriaHashMap;

    /**
     * Constructor of CriteriaMap
     * Create a hash map for storing criteria
     */
    public CriteriaMap () {
        criteriaHashMap = new HashMap<>();
    }

    /**
     * @param criName name of the criteria to be added
     * @param criteria the criteria
     */
    public void addCriteria (String criName, Criteria criteria) {
        criteriaHashMap.put(criName, criteria);
    }

    /**
     * @param criName name of the criteria
     * @return true or false considering whether the hashmap contains the corresponding criteria
     */
    public boolean hasSameCriteria (String criName) {
        return criteriaHashMap.containsKey(criName);
    }

    /**
     * @return the hash map of criteria
     */
    public HashMap<String, Criteria> getHashMap () {
        return criteriaHashMap;
    }

    /**
     * @param criName name of the criteira to be retrieved
     * @return a Criteria object
     */
    public Criteria getCriteria (String criName) {
        return criteriaHashMap.get(criName);
    }




}

