package hk.edu.polyu.comp.comp2021.cvfs;
import hk.edu.polyu.comp.comp2021.cvfs.model.CVFS;

/**
 * Application is for running the CVFS system
 * By starting the method in CVFS Controller
 */
public class Application {

    /**
     * the main entry method of CVFS system
     * @param args //
     */
    public static void main(String[] args){
        CVFS cvfs = new CVFS();
        CVFSView cvfsView = new CVFSView(cvfs);
        CVFSController cvfsController = new CVFSController(cvfs, cvfsView);
        cvfsController.commandLineInput();
    }
}
