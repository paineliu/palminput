import Foundation
import AudioToolbox
import UIKit

class SHomeCell: UICollectionViewCell {
    
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
        titleLabel = UILabel(frame: CGRect(x: 20, y: 0, width: 400, height: 50))
        titleLabel?.backgroundColor = UIColor.black
        titleLabel?.textColor = UIColor.white
        titleLabel?.font = UIFont.systemFont(ofSize: 20)
//        titleLabel?.textAlignment = NSTextAlignment.center
        self.addSubview(titleLabel!)
    }
}

class HandWriteView: UIView, UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout {
    
    func numberOfSectionsInCollectionView(collectionView: UICollectionView) -> Int {
        return 1
    }
    
    func collectionView(_ collectionView: UICollectionView,
                   layout collectionViewLayout: UICollectionViewLayout,
                   sizeForItemAt indexPath: IndexPath) -> CGSize {
        let s = pyProvider.getCandItem(Int32(indexPath.row))
        let size: CGSize = s!.size(withAttributes: [NSAttributedString.Key.font: UIFont.systemFont(ofSize: 20.0)])
        return CGSize(width: size.width + 30, height: 45)
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
        cell.titleLabel?.frame = CGRect(x: 20, y: 0, width: size.width + 31, height: 50)
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
            pyProvider.reset();
        }
        
        collectionView.isHidden = pyProvider.getCandTotal() == 0
        self._inlinelabel.text = pyProvider.getKeyString()
        collectionView.reloadData()
        collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.top, animated: false)
    }
    
    var pyProvider: PalmEngine!
    
        
    var inputController: UIInputViewController!
    var touchDict = [UITouch: [CGPoint]]()
    var labelDict = [String: String]()
    var list = [CGPoint]()
    var pointVal = CGPoint.zero
    
    var jsonFile:String?
    var jsonData:NSData?

    var skin:KeyboardSkin?
    
    var firstTimer:Timer!
    var secondTimer:Timer!

    var hideImg:UIImage!
    var backImg:UIImage!
    var spaceImg:UIImage!
    var menuImg:UIImage!
    var shiftImg:UIImage!
    var shiftUpperImg:UIImage!
    var shiftLockedImg:UIImage!
    var languageImg:UIImage!
    
    var _inlinelabel:UILabel!
    var collectionView:UICollectionView!
    var skinPath:String!
    var skinResPool: InputResPool!
    
    override init(frame: CGRect) {
        
        super.init(frame: frame)
        self.isMultipleTouchEnabled = true
        jsonFile = Bundle.main.path(forResource: "pinyin-qwerty", ofType: "json", inDirectory:"skin-black")
        jsonData = NSData.init(contentsOfFile: jsonFile!)
        let decoder = JSONDecoder()
        skinPath = "skin-black"
        skinResPool = InputResPool(skinPath: skinPath)
        labelDict["&backspace;"] = "backspace"
        labelDict["&hide;"] = "hide"
        labelDict["&space;"] = ""
        labelDict["&menu;"] = "menu"
        labelDict["&language;"] = "language"
        labelDict["&menu;"] = "menu"
        labelDict["&shift;"] = "shift_normal"
        labelDict["&shift_normal;"] = "shift_normal"
        labelDict["&shift_upper;"] = "shift_upper"
        labelDict["&shift_locked;"] = "shift_locked"

       
        do {
            skin = try decoder.decode(KeyboardSkin.self, from: jsonData! as Data)
        } catch {
            print(error)
        }
        skin?.resize(frame: frame)
        
        pyProvider = PalmEngine()
        pyProvider.reset()
        
        let frameTable = CGRect(x: 0, y:10, width: frame.width - 60, height: 45)
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
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        isMultipleTouchEnabled = true
    }
    
    public func setController(_ conroller: UIInputViewController) {
        self.inputController = conroller
    }
    
    func textHeight(text: String, font: UIFont?) -> CGFloat {
        let attributes = font != nil ? [NSAttributedString.Key.font: font!] : [:]
        return text.size(withAttributes: attributes).height
    }
    
    public func drawImage(img:UIImage?, rect:CGRect)
    {
        if (img != nil) {
            var sizeImg = img!.size
            var origImg = rect.origin
            sizeImg.width = sizeImg.width * 2 / 3
            sizeImg.height = sizeImg.height * 2 / 3
            
            origImg.x = origImg.x + (rect.size.width - sizeImg.width) / 2
            origImg.y = origImg.y + (rect.size.height - sizeImg.height) / 2
            
            
            let realRect = CGRect(origin: origImg, size:sizeImg)
            img!.draw(in: realRect)
        }
    }
    
   
    
    public func drawButton(_ context: CGContext, rect: CGRect, label:String) {
        //创建一个矩形，它的所有边都内缩3点
        let drawingRect = rect.insetBy(dx: 1, dy: 1)
        
        //创建并设置路径
        let path = CGMutablePath()
        //绘制矩形
        path.addRoundedRect(in: drawingRect, cornerWidth: 5, cornerHeight: 5)
        
        //添加路径到图形上下文
        context.addPath(path)
        context.clip()
        //设置笔触颜色
        context.setStrokeColor(UIColor.orange.cgColor)
        //设置笔触宽度
        context.setLineWidth(1)
        //设置填充颜色
        context.setFillColor(UIColor.blue.cgColor)
        
        //绘制路径并填充
        //context.drawPath(using: .fillStroke)
        //context.saveGState()
        //context.clip(to: [rect])
        
        //使用rgb颜色空间
        let colorSpace = CGColorSpaceCreateDeviceRGB()
        //颜色数组（这里使用三组颜色作为渐变）fc6820
        let compoents:[CGFloat] = [0xfc/255, 0x68/255, 0x20/255, 1,
                                   0xfe/255, 0xd3/255, 0x2f/255, 1,
                                   0xb1/255, 0xfc/255, 0x33/255, 1]
        //没组颜色所在位置（范围0~1)
        let locations:[CGFloat] = [0,0.5,1]
        //生成渐变色（count参数表示渐变个数）
        let gradient = CGGradient(colorSpace: colorSpace, colorComponents: compoents,
                                  locations: locations, count: locations.count)!
        
        //渐变开始位置
        let start = CGPoint(x: rect.minX, y: rect.minY)
        //渐变结束位置
        let end = CGPoint(x: rect.minX, y: rect.maxY)
        //绘制渐变
        context.drawLinearGradient(gradient, start: start, end: end,
                                   options: .drawsBeforeStartLocation)
        let startRadius = 0.0
        let endRadius = 40.0
        let startCenter = CGPoint(x:rect.minX, y:rect.minY)
        let endCenter = CGPoint(x:rect.midX, y:rect.midY)
        
        context.drawRadialGradient(gradient, startCenter: startCenter, startRadius: CGFloat(startRadius), endCenter: endCenter, endRadius: CGFloat(endRadius), options: .drawsBeforeStartLocation)
        context.resetClip()
        let shadow : NSShadow = NSShadow()
        shadow.shadowOffset = CGSize(width: 0.0, height: 1.0)
        let style = NSMutableParagraphStyle()
        style.alignment = .center
        let attributeDict: [NSAttributedString.Key : Any] = [
            //.font: UIFont(name: "HelveticaNeue-Thin", size: 24)!,
            .shadow:shadow,
            .font: UIFont.systemFont(ofSize:24, weight:UIFont.Weight.bold),
            .foregroundColor: UIColor.white,
            .paragraphStyle: style,
            ]
        let height = textHeight(text: label, font: UIFont.boldSystemFont(ofSize:24))
        var r = rect
        r.origin.y = r.origin.y + (r.size.height - height) / 2
        r.size.height = height
        label.draw(in: r, withAttributes: attributeDict);
        // label.draw(with: r, options:.usesLineFragmentOrigin, attributes:attributeDict, context:nil)
    }
    
    override func draw(_ rect: CGRect) {
        
        //2 - get the current context
        if let context = UIGraphicsGetCurrentContext() {
        
//            context.setAllowsAntialiasing(true)
//            //context.setLineWidth(5.0)
//            context.setFillColor(skin?.bgColor.cgColor() ?? UIColor.white.cgColor)
//            context.fill(rect)
//
//            if let buttons = skin?.buttons {
//                for b in buttons {
//                    var bPress = false
//                    for (_, value) in touchDict {
//
//                        if (b.realRect!.contains(value[0])) {
//                                                        
//                            bPress = true;
//                            break
//                        }
//                    }
//                    if (bPress) {
//                        drawButton(context, button:b, styleState:skin!.buttonStyles[b.styleId].press)
//                    }
//                    else {
//                        drawButton(context, button:b, styleState:skin!.buttonStyles[b.styleId].normal)
//                    }
//                }
//            }

            
            //绘制笔画
//            if list.count > 0 {
//                //创建并设置路径
//                let path = CGMutablePath()
//                //var p : CGPoint
//                path.move(to: list[0])
//                for p in list {
//                    path.addLine(to: p)
//                }
//                context.setStrokeColor(UIColor.orange.cgColor)
//                //设置笔触宽度
//                context.setLineWidth(6)
//                //设置填充颜色
//                context.setFillColor(UIColor.blue.cgColor)
//
//                //添加路径到图形上下文
//                context.addPath(path)
//                context.drawPath(using: .stroke)
//            }

//            let priceString:NSString = NSString(format: "%.02f,%.02f - %.02f,%.02f", rect.width, rect.height, pointVal.x, pointVal.y)
//            priceString.draw(at: CGPoint(x: 5, y: rect.height - 20), withAttributes: nil);
        }
    }
    
    @objc func tickDown2()
    {
        
        if self.inputController != nil && (self.inputController.textDocumentProxy.documentContextBeforeInput) != nil {
            AudioServicesPlaySystemSound(1155)
            self.inputController.textDocumentProxy.deleteBackward()
        }
        
    }
    
    @objc func tickDown()
    {
        secondTimer = Timer.scheduledTimer(timeInterval: 0.1,
                                          target:self,selector:#selector(HandWriteView.tickDown2),
                                          userInfo:nil,repeats:true)
    }
    
    func playKeySound(pointVal: CGPoint) {
        //        1123——键盘字母按钮按下声音；
        //        1155——退格键声音；
        //        1156——其他键盘功能键声音；
        //        1519——6s peek 震动效果；
        //        1520——6s pop 震动效果；
        //        1521——6s 三连震 震动效果
        var sid = 0
        
        if let buttons = skin?.buttons {
            for b in buttons {
                let rect = b.realRect!
                let id = b.id
                
                if rect.contains(pointVal) {
                    setNeedsDisplay(b.dispRect!)
                    switch id {
                    case "VK_SPACE", "VK_DONE":
                        sid = 1156
                        
                    case "VK_BACKSPACE":
                        if (self.inputController != nil) {
                            self.inputController.textDocumentProxy.deleteBackward()
                        }
                        sid = 1155
                        firstTimer = Timer.scheduledTimer(timeInterval: 0.15,
                                                     target:self,selector:#selector(HandWriteView.tickDown),
                                                     userInfo:nil,repeats:false)
                    default:
                        sid = 1123
                    }
                }

            }
        }
        if (sid != 0) {
            AudioServicesPlaySystemSound(SystemSoundID(sid))
            UIDevice.current.playInputClick()
        
        }
    }
    
    func keyPress(touch:UITouch, point:CGPoint) {
        var caretPos: Int32
        caretPos = 0

        if let buttons = skin?.buttons {
            let value = touchDict[touch]!
            for b in buttons {
                if (b.realRect!.contains(value[0])) {
                    print(b.label)
                    switch b.label {
                    case "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "\'":
                        let key = (b.label.lowercased() as NSString).character(at: 0)
                        pyProvider.processKey(key)
                        self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: true)
                        collectionView.isHidden = pyProvider.getCandTotal() == 0
                        collectionView.reloadData()
                        collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.left, animated: false)
                        break
                    case "&backspace;":
                        pyProvider.processKey(8)
                        self._inlinelabel.text = pyProvider.getCompString(&caretPos, withSplit: true)
                        collectionView.isHidden = pyProvider.getCandTotal() == 0
                        collectionView.reloadData()
                        collectionView.scrollToItem(at: IndexPath(item: 0, section: 0), at: UICollectionView.ScrollPosition.left, animated: false)
                    default:
                        break
                    }
                    break
                }
            }
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?){
        
        for touch in touches {
            let pointVal = touch.location(in: self)
            touchDict.removeValue(forKey: touch)
            touchDict.updateValue([pointVal], forKey: touch)
            playKeySound(pointVal: pointVal)
        }
    
        setNeedsDisplay()
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            let pointVal = touch.location(in: self)
            touchDict[touch]?.append(pointVal)
        }
        setNeedsDisplay()
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            pointVal = touch.location(in: self)
            keyPress(touch: touch, point: pointVal)
            touchDict.removeValue(forKey: touch)
        }
        
        if let buttons = skin?.buttons {
            for b in buttons {
                let rect = b.realRect!
                let label = b.label
                
                if rect.contains(pointVal) {
                    switch label {
                    case "1", "2", "3", "4", "5":
                        if (inputController != nil) {
                            self.inputController.textDocumentProxy.insertText(label)
                        }
                    case "完成":
                        if (inputController != nil) {
                            self.inputController.dismissKeyboard()
                        }
                    case "<back>":
                        break
                    case "空格":
                        if (inputController != nil) {
                            self.inputController.textDocumentProxy.insertText(" ")
                        }
                    case ":":
                        if (inputController != nil) {
                            self.inputController.advanceToNextInputMode()
                        }
                    default:
                        if (inputController != nil) {
                            self.inputController.textDocumentProxy.insertText(label)
                        }
                    }
                }
            }
        }
        
        if (firstTimer != nil) && firstTimer.isValid {
            firstTimer.invalidate()
        }
        if (secondTimer != nil) && secondTimer.isValid {
            secondTimer.invalidate()
        }
        setNeedsDisplay()
    }

    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            let t = touch
            let fingerPoint = t.location(in: self)
            pointVal = fingerPoint
            list.append(pointVal)
        }
        list.removeAll()
        setNeedsDisplay()
    }
}
