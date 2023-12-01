import Foundation
import AudioToolbox
import UIKit

class PinyinCandCell: UICollectionViewCell {
    
    let width = 400//获取屏幕宽
    var titleLabel:UILabel?//title
    
    override init(frame: CGRect) {
        super.init(frame: frame)
        initView()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func initView(){
        self.backgroundColor = UIColor.black
        titleLabel = UILabel(frame: CGRect(x: 20, y: 0, width: 400, height: 48))
        titleLabel?.backgroundColor = UIColor.black
        titleLabel?.textColor = UIColor.white
        titleLabel?.font = UIFont.systemFont(ofSize: 20)
//        titleLabel?.textAlignment = NSTextAlignment.center
        self.addSubview(titleLabel!)
    }
}

class PinyinView: InputBaseView, UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout {
    
    func numberOfSectionsInCollectionView(collectionView: UICollectionView) -> Int {
        return 1
    }
    
    func collectionView(_ collectionView: UICollectionView,
                   layout collectionViewLayout: UICollectionViewLayout,
                   sizeForItemAt indexPath: IndexPath) -> CGSize {
        let s = pyProvider.getCandItem(Int32(indexPath.row))
        let size: CGSize = s!.size(withAttributes: [NSAttributedString.Key.font: UIFont.systemFont(ofSize: 20.0)])
        return CGSize(width: size.width + 30, height: 43)
    }
    
    //返回多少个cell
    func collectionView(_ collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        return Int(pyProvider.getCandTotal())
    }

    //返回自定义的cell
    func collectionView(_ collectionView: UICollectionView, cellForItemAt indexPath: IndexPath) -> UICollectionViewCell {
        let cell = collectionView.dequeueReusableCell(withReuseIdentifier: "cell", for: indexPath) as! SHomeCell
        let s = pyProvider.getCandItem(Int32(indexPath.row))
        cell.titleLabel?.text = s
        let size: CGSize = s!.size(withAttributes: [NSAttributedString.Key.font: UIFont.systemFont(ofSize: 20.0)])
        cell.titleLabel?.frame = CGRect(x: 20, y: 0, width: size.width + 31, height: 48)
        if #available(iOSApplicationExtension 13.0, *) {
            cell.titleLabel?.backgroundColor = UIColor.init(cgColor: skin!.bgColor.cgColor())
            cell.backgroundColor = UIColor.init(cgColor: skin!.bgColor.cgColor())
            
        } else {
            // Fallback on earlier versions
        }

        return cell
    }
    
    func collectionView(_ collectionView: UICollectionView, didSelectItemAt indexPath: IndexPath) {
        let strCand = pyProvider.selCandItem(Int32(indexPath.row))
        if (strCand != nil) {
            delegate.insertText(strCand!)
            pyProvider.reset();
        }
        
        collectionView.isHidden = pyProvider.getCandTotal() == 0
        self._inlinelabel.text = pyProvider.getKeyString()
        delegate.setMarkedText(text: pyProvider.getKeyString())
        collectionView.reloadData()
        collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.top, animated: false)
        setNeedsDisplay()
    }
    
    var pyProvider: PalmEngine!
    var _inlinelabel:UILabel!
    var collectionView:UICollectionView!
    
    override init(frame: CGRect, jsonFile:String, delegate:KeyboardMgrProtocol) {
        
        super.init(frame: frame, jsonFile:jsonFile, delegate:delegate)
        self.isMultipleTouchEnabled = true

        
        pyProvider = PalmEngine()
        pyProvider.reset()
        
        let frameTable = CGRect(x: 0, y:10, width: frame.width - 56, height: 43)
        let layout = UICollectionViewFlowLayout()
        layout.scrollDirection = UICollectionView.ScrollDirection.horizontal;
        layout.minimumInteritemSpacing = 0
        collectionView = UICollectionView(frame: frameTable, collectionViewLayout: layout)
        //注册一个cell
        collectionView.register(SHomeCell.self, forCellWithReuseIdentifier:"cell")
        if #available(iOSApplicationExtension 13.0, *) {
            collectionView.backgroundColor = UIColor.init(cgColor: skin!.bgColor.cgColor())
        } else {
            // Fallback on earlier versions
        }
        collectionView.delegate = self;
        collectionView.dataSource = self;
        collectionView.alwaysBounceVertical = false
        collectionView.alwaysBounceHorizontal = true
        collectionView.showsHorizontalScrollIndicator = false
        collectionView.showsVerticalScrollIndicator = false

        let frameL = CGRect(x: 10, y:0, width: frame.width, height: 16)
        self._inlinelabel = UILabel(frame: frameL)
        self._inlinelabel.font = UIFont.systemFont(ofSize: 14.0)
        self._inlinelabel.textColor = UIColor.white
        self.addSubview(collectionView)
        self.insertSubview(_inlinelabel, aboveSubview:collectionView)
//        label.isHidden = true
        collectionView.isHidden = true
    }
    
    override public func getReturnKeyString() -> String {
        if (pyProvider.getCandTotal() > 0) {
            return "确认"
        }
        
        return delegate.getReturnKeyString()
    }
    
    override func onKeyRepeat(button: KeyboardButton){
        var caretPos: Int32
        caretPos = 0
        if (button.id == "VK_BACKSPACE") {
            if (pyProvider.getCandTotal() > 0) {
                pyProvider.processKey(8)
                self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: true)
                delegate.setMarkedText(text: self._inlinelabel.text ?? "")
                collectionView.isHidden = pyProvider.getCandTotal() == 0
                collectionView.reloadData()
                collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.left, animated: false)
            }
            else {
                super.onKeyUp(button: button)
            }
        }
    }

    override func onKeySlide(button: KeyboardButton, type: KeyEventType){
        if (button.id == "VK_BACKSPACE" && type == .SlideLeft) {
            pyProvider.reset();
            collectionView.isHidden = true
            self._inlinelabel.text = pyProvider.getKeyString()
            delegate.setMarkedText(text: pyProvider.getKeyString())
        }
        else {
            if (pyProvider.getCandTotal() == 0) {
                super.onKeySlide(button: button, type: type)
            }
        }
    }

    override func onKeyUp(button: KeyboardButton){
        var caretPos: Int32
        caretPos = 0
        switch button.id {
        case "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "\'":
            let key = (button.label.lowercased() as NSString).character(at: 0)
            pyProvider.processKey(key)
            self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: true)
            delegate.setMarkedText(text: self._inlinelabel.text ?? "")
            collectionView.isHidden = pyProvider.getCandTotal() == 0
            collectionView.reloadData()
            collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.left, animated: false)
            break
        case "VK_BACKSPACE":
            if (pyProvider.getCandTotal() > 0) {
                pyProvider.processKey(8)
                self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: true)
                delegate.setMarkedText(text: self._inlinelabel.text ?? "")
                collectionView.isHidden = pyProvider.getCandTotal() == 0
                collectionView.reloadData()
                collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.left, animated: false)
            }
            else {
                super.onKeyUp(button: button)
            }
        case "VK_SPACE":
            if (pyProvider.getCandTotal() > 0) {
                let strCand = pyProvider.selCandItem(0)
                if (strCand != nil) {
                    delegate.insertText(strCand!)
                    pyProvider.reset();
                }
                
                collectionView.isHidden = true
                self._inlinelabel.text = ""
            }
            else {
                super.onKeyUp(button: button)
            }
        case "VK_DONE":
            if (pyProvider.getCandTotal() > 0) {
                
                self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: false)
                delegate.insertText(self._inlinelabel.text ?? "")
                pyProvider.reset()
                self._inlinelabel.text = ""
                collectionView.isHidden = true
            }
            else {
                delegate.insertText("\n")
            }
        default:
            super.onKeyUp(button: button)
        }
    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        isMultipleTouchEnabled = true
    }
}
