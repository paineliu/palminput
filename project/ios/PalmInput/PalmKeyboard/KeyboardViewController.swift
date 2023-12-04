import UIKit

class KeyboardViewController: UIInputViewController {

    @IBOutlet var nextKeyboardButton: UIButton!
    
    var hwView: InputMgrView!
    
    let portraitHeight:CGFloat = 256.0
    let landscapeHeight:CGFloat = 203.0
    var heightConstraint: NSLayoutConstraint?
    var conHeight:CGFloat!
    
    override func updateViewConstraints() {
        super.updateViewConstraints()
        
        if (self.view.frame.size.width == 0 ||
            self.view.frame.size.height == 0) {
            return
        }
        
        var isLandscape = false


        let vClass = self.traitCollection.verticalSizeClass
        print(vClass)
        switch (vClass) {
        case .compact :
            print("v compact");
            isLandscape = true
            // horizontal is compact class.. do stuff...
            break;
        case .regular:
            isLandscape = false
            print("v regular");
            // horizontal is regular class.. do stuff...
            break;
        default :
            print("v unknown")
            // horizontal is unknown..
            break;
        }
        
        let hClass = self.traitCollection.horizontalSizeClass
        print(hClass)
//        switch (hClass) {
//        case .compact :
//            print("h compact");
//            isLandscape = true
//            // horizontal is compact class.. do stuff...
//            break;
//        case .regular :
//            isLandscape = false
//            print("h regular");
//            // horizontal is regular class.. do stuff...
//            break;
//        default :
//            print("h unknown")
//            // horizontal is unknown..
//            break;
//        }

        isLandscape = self.interfaceOrientation != UIInterfaceOrientation.portrait
        
        if (isLandscape) {
            print("land", self.view.frame.size.width, self.view.frame.size.height);
            conHeight = landscapeHeight
            heightConstraint?.constant = landscapeHeight
            //view.addConstraint(heightConstraint!)
        }
        else {
            print("port", self.view.frame.size.width, self.view.frame.size.height);
            conHeight = portraitHeight
            heightConstraint?.constant = portraitHeight
            
            //view.addConstraint(heightConstraint!)
        }
    }

    override func viewDidAppear(_ animated: Bool)
    {
        print("viewDidAppear\n")
        
        super.viewDidAppear(animated)
        //self.hwView.setNeedsDisplay()
        //setUpHeightConstraint()
    }
    
    override func viewWillLayoutSubviews(){
        super.viewWillLayoutSubviews()
        updateViewConstraints()
        print("viewWillLayoutSubviews\n");
    }
    override func viewDidLayoutSubviews() {
        print("viewDidLayoutSubviews\n");
        self.hwView.setNeedsDisplay()
    }
    override func viewDidLoad() {
        super.viewDidLoad()
        if let userDefaults = UserDefaults(suiteName: "group.bitinput.freeinput.com") {
            let skinName = userDefaults.string(forKey: "flutter.skin_name") ?? "skin-white"
            let fuzzyFlags = userDefaults.integer(forKey: "flutter.py_fuzzy_flags")
            let predictEnable = userDefaults.bool(forKey: "flutter.predict_enable")
            print(skinName)
            print(fuzzyFlags)
            print(predictEnable)
        }

        print("didload\n");
        hwView = InputMgrView(frame:CGRect(x: 0, y: 0,width: 395, height: 256))
        hwView.setController(self)
        hwView.show(inputKeyboard: .Pinyin)
        hwView.translatesAutoresizingMaskIntoConstraints = false
        self.view.addSubview(hwView)
        hwView.autoresizingMask = [UIView.AutoresizingMask.flexibleWidth, UIView.AutoresizingMask.flexibleHeight]
        // Perform custom UI setup here
        self.nextKeyboardButton = UIButton(type: .system)
        
        //self.nextKeyboardButton.setTitle(NSLocalizedString("Next Keyboard", comment: "Title for 'Next Keyboard' button"), for: [])
        //self.nextKeyboardButton.sizeToFit()
        self.nextKeyboardButton.translatesAutoresizingMaskIntoConstraints = false
        //self.nextKeyboardButton.addTarget(self, action: #selector(handleInputModeList(from:with:)), for: .allTouchEvents)
        nextKeyboardButton.isHidden = true
        self.view.addSubview(self.nextKeyboardButton)
        
        //self.nextKeyboardButton.leftAnchor.constraint(equalTo: self.view.leftAnchor).isActive = true
        //self.nextKeyboardButton.bottomAnchor.constraint(equalTo: self.view.bottomAnchor).isActive = true
        
        if conHeight == nil {
            conHeight = portraitHeight
        }
        heightConstraint = NSLayoutConstraint(item: self.view, attribute: NSLayoutConstraint.Attribute.height, relatedBy: NSLayoutConstraint.Relation.equal, toItem: nil, attribute: NSLayoutConstraint.Attribute.notAnAttribute, multiplier: 1.0, constant: conHeight)
        
        heightConstraint!.priority = UILayoutPriority(rawValue: 999.0)
        view.addConstraint(heightConstraint!)
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated
    }
    
    override func textWillChange(_ textInput: UITextInput?) {
        // The app is about to change the document's contents. Perform any preparation here.
        print("textWillChange\n");
    }
    
    override func textDidChange(_ textInput: UITextInput?) {
        // The app has just changed the document's contents, the document context has been updated.

        print("textDidChange\n");
        self.hwView.setNeedsDisplay()

//        var textColor: UIColor
//        let proxy = self.textDocumentProxy
//        if proxy.keyboardAppearance == UIKeyboardAppearance.dark {
//            textColor = UIColor.white
//        } else {
//            textColor = UIColor.black
//        }
        //self.nextKeyboardButton.setTitleColor(textColor, for: [])
    }

}
