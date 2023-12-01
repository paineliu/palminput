import Foundation
import AudioToolbox
import UIKit

class InputBaseView: UIView {

    enum KeyEventType {
        case Down
        case Repeat
        case Up
        case SlideUp
        case SlideDown
        case SlideLeft
        case SlideRight
    }
    enum ShiftMode {
        case Low
        case Up
        case UpLock
    }

    var inputController: UIInputViewController!
    var touchDict = [UITouch: [CGPoint]]()
    var labelDict = [String: String]()
    var pointVal = CGPoint.zero
    
    var jsonFilename:String?
    var jsonData:NSData?

    var skin:KeyboardSkin?
    var symLock:Bool!
    var shiftMode:ShiftMode!
    var firstTimer:Timer!
    var secondTimer:Timer!
    var skinPath:String!
    var skinResPool: InputResPool!
    var delegate:KeyboardMgrProtocol!
    var repeatButton: KeyboardButton!
    
    init(frame: CGRect, jsonFile: String, delegate:KeyboardMgrProtocol) {
        
        super.init(frame: frame)
        self.delegate = delegate
        self.isMultipleTouchEnabled = true
        skinPath = "skin-black"
        let name = jsonFile + ".json"
        jsonFilename = Bundle.main.path(forResource: jsonFile, ofType: nil)
        	
        jsonData = NSData(contentsOfFile: name)
        let decoder = JSONDecoder()
        symLock = false
        shiftMode = .Low
        skinResPool = InputResPool(skinPath: skinPath)
        labelDict["&backspace;"] = "backspace"
        labelDict["&hide;"] = "hide"
        labelDict["&space;"] = "space"
        labelDict["&emoji;"] = "emoji"
        labelDict["&menu;"] = "menu"
        labelDict["&english;"] = "cn_en"
        labelDict["&chinese;"] = "en_cn"
        labelDict["&language;"] = "language"
        labelDict["&lock;"] = "lock"
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
    
    public func getImageFilename(label:String) -> String? {
        if (label == "&lock;" && symLock) {
            return "unlock"
        }
        
        if (label == "&shift;") {
            switch shiftMode {
            case .Low:
                return "shift_normal"
            case .Up:
                return "shift_upper"
            case .UpLock:
                return "shift_locked"
            case .none:
                return "shift_normal"
            }
        }
        
        return labelDict[label]
    }
    
    public func getReturnKeyString() -> String {
        return delegate.getReturnKeyString()
    }
    
    public func drawButton(_ context: CGContext, button: KeyboardButton, styleState: KeyboardButtonStyleState) {

        let drawingRect = button.dispRect!.insetBy(dx: CGFloat(styleState.btnPadding.left / 2), dy: CGFloat(styleState.btnPadding.top / 2))
                
        //保存绘制设置
        context.saveGState()
         
        //准备阴影
        if #available(iOSApplicationExtension 13.0, *) {
            let shadowColor = styleState.btnShadow.color.cgColor()
            let shadowOffet = CGSize(width: styleState.btnShadow.dx, height: styleState.btnShadow.dy)
            let shadowBlurRadius = styleState.btnShadow.radius
             
            //创建和应用阴影
            context.setShadow(offset: shadowOffet, blur: CGFloat(shadowBlurRadius), color: shadowColor)
            //绘制带有阴影的路径

            //let path1 = UIBezierPath(rect: drawingRect.insetBy(dx: 5, dy: 5))
            if (styleState.btnBorder.width > 0) {
                let path1 = UIBezierPath(roundedRect: drawingRect, cornerRadius: CGFloat(styleState.btnBorder.rx / 2))
                path1.lineWidth = CGFloat(styleState.btnBorder.width / 2)
                context.setStrokeColor(styleState.btnBorder.color.cgColor());
                context.setFillColor(styleState.btnColor.cgColor())
                path1.fill()
                path1.stroke()
            }
            
            
            //还原绘制设置
            context.restoreGState()
             
            if let filename = getImageFilename(label: button.label) {
                drawImage(img: skinResPool.getImage(filename: filename), rect: drawingRect)
            } else {
                var labelText = button.label
                
                if (button.label == "&done;") {
                    
                    labelText = getReturnKeyString()
                }
                
                if (shiftMode != .Low) {
                    labelText = labelText.uppercased()
                }
                
                //设置笔触颜色
                
                let shadow : NSShadow = NSShadow()
                shadow.shadowOffset = CGSize(width: 0.0, height: 0.0)
                let style = NSMutableParagraphStyle()
                style.alignment = .center
                let attributeDict: [NSAttributedString.Key : Any] = [
                    //.font: UIFont(name: "HelveticaNeue-Thin", size: 24)!,
                    .font: UIFont.systemFont(ofSize:CGFloat(styleState.labelFont.size / 2), weight:UIFont.Weight.bold),
                    .shadow:shadow,
                    .foregroundColor: UIColor.init(cgColor: styleState.labelFont.color.cgColor()),
                    .paragraphStyle: style,
                    ]
                let height = textHeight(text: labelText, font: UIFont.boldSystemFont(ofSize:CGFloat(styleState.labelFont.size / 2)))
                var labelRect = drawingRect
                labelRect.origin.y = labelRect.origin.y + (labelRect.size.height - height) / 2
                labelRect.size.height = height
                labelText.draw(in: labelRect, withAttributes: attributeDict);
                
                if let tagText = button.tag {
                    let shadow : NSShadow = NSShadow()
                    shadow.shadowOffset = CGSize(width: 0.0, height: 0.0)
                    let style = NSMutableParagraphStyle()
                    style.alignment = .center
                    let attributeDict: [NSAttributedString.Key : Any] = [
                        //.font: UIFont(name: "HelveticaNeue-Thin", size: 24)!,
                        .font: UIFont.systemFont(ofSize:CGFloat(styleState.tagFont.size / 2), weight:UIFont.Weight.light),
                        .shadow:shadow,
                        .foregroundColor: UIColor.init(cgColor: styleState.tagFont.color.cgColor()),
                        .paragraphStyle: style,
                        ]
                    let tagHeight = textHeight(text: labelText, font: UIFont.boldSystemFont(ofSize:CGFloat(styleState.tagFont.size / 2)))
                    var tagRect = drawingRect
                    
                    tagRect.origin.y = tagRect.origin.y + (tagRect.size.height - tagHeight) / 20
                    tagRect.size.height = tagHeight
                    tagText.draw(in: tagRect, withAttributes: attributeDict);
                }
            }
            
        } else {
            // Fallback on earlier versions
        }

    }
    
    override func draw(_ rect: CGRect) {
        
        if let context = UIGraphicsGetCurrentContext() {
        
            context.setAllowsAntialiasing(true)
            if #available(iOSApplicationExtension 13.0, *) {
                context.setFillColor(skin?.bgColor.cgColor() ?? UIColor.white.cgColor)
            } else {
                // Fallback on earlier versions
            }
            context.fill(rect)

            if let buttons = skin?.buttons {
                for b in buttons {
                    var bPress = false
                    for (_, value) in touchDict {

                        if (b.realRect!.contains(value[0])) {
                            bPress = true;
                            break
                        }
                    }
                    
                    if (bPress) {
                        drawButton(context, button:b, styleState:skin!.buttonStyles[b.styleId].press)
                    }
                    else {
                        drawButton(context, button:b, styleState:skin!.buttonStyles[b.styleId].normal)
                    }
                }
            }
        }
    }
    
    @objc func tickDown2()
    {
        
        onKeyRepeat(button: repeatButton)
        
    }
    
    @objc func tickDown()
    {
        secondTimer = Timer.scheduledTimer(timeInterval: 0.1,
                                          target:self,selector:#selector(self.tickDown2),
                                          userInfo:nil,repeats:true)
    }
    
    func getTouchDownButton(touch:UITouch) -> KeyboardButton? {
        if let value = touchDict[touch] {
            if let buttons = skin?.buttons {
                for button in buttons {
                    if (button.realRect!.contains(value[0])) {
                        return button
                    }
                }
            }
        }

        return nil
    }

    func getTouchUpButton(touch:UITouch) -> KeyboardButton? {
        if let value = touchDict[touch] {
            if (value.count > 1) {
                if let buttons = skin?.buttons {
                    for button in buttons {
                        if (button.realRect!.contains(value[value.count - 1])) {
                            return button
                        }
                    }
                }
            }
        }

        return nil
    }

    
    func playKeySound(button: KeyboardButton) {

        var sid = 0
        
        switch button.id {
        case "VK_SPACE", "VK_DONE":
            sid = 1156 // 其他键盘功能键声音；
        case "VK_BACKSPACE":
            sid = 1155 // 退格键声音
        default:
            sid = 1123 // 键盘字母按钮按下声音；
        }

        if (sid != 0) {
            AudioServicesPlaySystemSound(SystemSoundID(sid))
            //UIDevice.current.playInputClick()
        }
    }
    
    func onTouchDown(touch:UITouch) {
        if let button = getTouchDownButton(touch: touch) {
            onKeyEvent(button: button, type: .Down)
        }
    }
    
    func startTimer(button: KeyboardButton) {
        repeatButton = button
        firstTimer = Timer.scheduledTimer(timeInterval: 0.15,
                                     target:self,selector:#selector(self.tickDown),
                                     userInfo:nil,repeats:false)
    }
    
    func stopTimer() {
        if (firstTimer != nil) && firstTimer.isValid {
            firstTimer.invalidate()
        }
        
        if (secondTimer != nil) && secondTimer.isValid {
            secondTimer.invalidate()
        }
    }
    
    func onKeyDown(button: KeyboardButton){
        playKeySound(button: button)
        if (button.id == "VK_BACKSPACE") {
            startTimer(button: button)
        }
    }

    func onKeyUp(button: KeyboardButton){
        // print(button.id)
        if (button.id == "VK_ENGLISH"){
            delegate.switchToKeyboard(inputKeyboard: .English)
        }
        if (button.id == "VK_CHINESE"){
            delegate.switchToKeyboard(inputKeyboard: .Chinese)
        }
        if (button.id == "VK_NUMBER"){
            delegate.switchToKeyboard(inputKeyboard: .SymbolNumber)
        }
        if (button.id == "VK_SYMBOL_CN"){
            delegate.switchToKeyboard(inputKeyboard: .SymbolCn)
        }
        if (button.id == "VK_SYMBOL_CN_MORE"){
            delegate.switchToKeyboard(inputKeyboard: .SymbolCnMore)
        }
        if (button.id == "VK_SYMBOL_EN"){
            delegate.switchToKeyboard(inputKeyboard: .SymbolEn)
        }
        if (button.id == "VK_SYMBOL_EN_MORE"){
            delegate.switchToKeyboard(inputKeyboard: .SymbolEnMore)
        }
        if (button.id == "VK_PREV_KEYBOARD") {
            delegate.backMainKeyboard()
        }
        if (button.id == "VK_BACKSPACE") {
            delegate.deleteBackward()
        }
        if (button.id == "VK_LANGUAGE") {
            delegate.nextInputMode()
        }
        if (button.id == "VK_SPACE") {
            delegate.insertText(" ")
        }
        if (button.id == "VK_HIDE") {
            delegate.hideKeyboard()
        }
        if (button.id == "VK_DONE") {
            delegate.insertText("\n")
        }
        if (button.id == "VK_SHIFT"){
            if (shiftMode == .Low) {
                shiftMode = .Up
            }
            else if (shiftMode == .Up) {
                shiftMode = .UpLock
            }
            else {
                shiftMode = .Low
            }
        }
        
        if (button.id == "VK_LOCK") {
            symLock = !symLock
        }
        
        if (button.id.uppercased() == button.label.uppercased()) {
            if (shiftMode != .Low){
                delegate.insertText(button.label.uppercased())
            }
            else {
                delegate.insertText(button.label)
            }
            if (shiftMode == .Up)
            {
                shiftMode = .Low;
            }
        }
    }
    
    func onKeyRepeat(button: KeyboardButton){
        if (button.id == "VK_BACKSPACE") {
            delegate.deleteBackward()
        }
    }

    func onKeySlide(button: KeyboardButton, type: KeyEventType){
        if (type == .SlideUp) {
            if let label = button.tag {
                delegate.insertText(label)
            }
        }
        else if (type == .SlideDown) {
            if button.tag != nil {
                delegate.insertText(button.label.uppercased())
            }
        }
    }

    func onKeyEvent(button: KeyboardButton, type: KeyEventType) {
        
        switch type {
        case .Down:
            onKeyDown(button: button)
        case .Up:
            onKeyUp(button: button)
        case .Repeat:
            onKeyRepeat(button: button)
        case .SlideUp, .SlideDown, .SlideLeft, .SlideRight:
            onKeySlide(button: button, type: type)
        }

    }
    
    func getTouchKeyEvent(touch:UITouch) ->(button: KeyboardButton, type: KeyEventType)? {
        if let buttonDown = getTouchDownButton(touch: touch) {
            if let buttonUp = getTouchUpButton(touch: touch) {
                if (buttonDown.id == buttonUp.id) {
                    return (button: buttonDown, type: KeyEventType.Up)
                }
            }
            if let value = touchDict[touch] {
                let p0 = value[0]
                let p1 = value[value.count - 1]
                var eventType:KeyEventType
                
                if abs(p0.x - p1.x) > abs(p0.y - p1.y)
                {
                    if (p0.x > p1.x) {
                        eventType = .SlideLeft
                    }
                    else{
                        eventType = .SlideRight
                    }
                }
                else {
                    if (p0.y > p1.y) {
                        eventType = .SlideUp
                    }
                    else{
                        eventType = .SlideDown
                    }
                }
                return (button: buttonDown, type: eventType)
            }
        }
        
        return nil
    }

    func onTouchUp(touch:UITouch) {
        if let result = getTouchKeyEvent(touch: touch) {
            onKeyEvent(button: result.button, type: result.type)
        }
    }
    
    override func touchesBegan(_ touches: Set<UITouch>, with event: UIEvent?){
        
        for touch in touches {
            let pointVal = touch.location(in: self)
            touchDict.removeValue(forKey: touch)
            touchDict.updateValue([pointVal], forKey: touch)
            onTouchDown(touch: touch)
        }
    
        setNeedsDisplay()
    }
    
    override func touchesMoved(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            let pointVal = touch.location(in: self)
            touchDict[touch]?.append(pointVal)
        }
        //setNeedsDisplay()
    }
    
    override func touchesEnded(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            pointVal = touch.location(in: self)
            touchDict[touch]?.append(pointVal)
            onTouchUp(touch: touch)
            touchDict.removeValue(forKey: touch)
        }
        
        stopTimer()
        setNeedsDisplay()
    }

    override func touchesCancelled(_ touches: Set<UITouch>, with event: UIEvent?){
        for touch in touches {
            touchDict.removeValue(forKey: touch)
        }
        
        stopTimer()
        setNeedsDisplay()
    }
}
