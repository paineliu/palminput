import Foundation
import AudioToolbox
import UIKit

enum InputKeyboard {
    case Chinese
    case Pinyin
    case Handwrite
    case English
    case PhoneNum
    case Number
    case SymbolNumber
    case SymbolCn
    case SymbolCnMore
    case SymbolEn
    case SymbolEnMore
}

protocol KeyboardMgrProtocol {
    func switchToKeyboard(inputKeyboard:InputKeyboard)
    func backMainKeyboard()
    func hideKeyboard()
    func nextInputMode()
    func handleInputModeList(from view: UIView, with event: UIEvent)
    func insertText(_ text: String)
    func setMarkedText(text: String)
    func deleteBackward()
    func getReturnKeyString() -> String
}

class InputMgrView: UIView {

    var inputController: UIInputViewController!
    var skinResPool: InputResPool!
    var keyboardDict = [InputKeyboard: InputBaseView]()
    
    var cnKeyboard:InputKeyboard!
    var mainKeyboard:InputKeyboard!
    
    override init(frame: CGRect) {
        
        super.init(frame: frame)
        self.isMultipleTouchEnabled = true
        cnKeyboard = .Pinyin
        mainKeyboard = .Pinyin

    }
    
    required init?(coder aDecoder: NSCoder) {
        super.init(coder: aDecoder)
        isMultipleTouchEnabled = true
    }
    
    public func setController(_ conroller: UIInputViewController) {
        self.inputController = conroller
    }
    
    public func createKeyboad(inputKeyboard:InputKeyboard, frame:CGRect) -> InputBaseView {
        let frame = CGRect(x: 0, y:0, width: 375, height: 256)
        switch inputKeyboard {
        case .Chinese:
            mainKeyboard = inputKeyboard
            if (cnKeyboard == .Pinyin) {
                if (inputController != nil) {
                    if (!self.inputController.needsInputModeSwitchKey) {
                        return PinyinView(frame: frame, jsonFile: "pinyin-qwerty", delegate:self)
                    }
                }
                
                return PinyinView(frame: frame, jsonFile: "pinyin-qwerty-ios", delegate:self)
            }
            else if (cnKeyboard == .Handwrite) {
                return InputBaseView(frame: frame, jsonFile: "handwrite", delegate:self)
            }
            else {
                return PinyinView(frame: frame, jsonFile: "pinyin-qwerty-ios", delegate:self)
            }
        case .SymbolEn:
            return InputBaseView(frame: frame, jsonFile: "symbol-qwerty-en", delegate:self)
        case .SymbolCn:
            return InputBaseView(frame: frame, jsonFile: "symbol-qwerty-cn", delegate:self)
        case .SymbolNumber:
            return InputBaseView(frame: frame, jsonFile: "symbol-number", delegate:self)
        case .SymbolEnMore:
            return InputBaseView(frame: frame, jsonFile: "symbol-qwerty-en-more", delegate:self)
        case .SymbolCnMore:
            return InputBaseView(frame: frame, jsonFile: "symbol-qwerty-cn-more", delegate:self)
        case .English:
            mainKeyboard = inputKeyboard
            return InputBaseView(frame: frame, jsonFile: "english-qwerty-ios", delegate:self)
        case .Number, .PhoneNum:
            mainKeyboard = inputKeyboard
            return InputBaseView(frame: frame, jsonFile: "number", delegate:self)
        default:
            mainKeyboard = .Pinyin
            if (inputController != nil) {
                if (!self.inputController.needsInputModeSwitchKey) {
                    return PinyinView(frame: frame, jsonFile: "pinyin-qwerty-ios", delegate:self)
                }
            }

            return PinyinView(frame: frame, jsonFile: "pinyin-qwerty-ios", delegate:self)
        }
    }
    
    public func show(inputKeyboard:InputKeyboard) {
        if let keyboardView = keyboardDict[inputKeyboard] {
            self.bringSubviewToFront(keyboardView)
        }
        else {

            let keyboardView = createKeyboad(inputKeyboard:inputKeyboard, frame: frame)
            self.addSubview(keyboardView)
            self.bringSubviewToFront(keyboardView)
        }
    }
        
    public func hide() {
        
    }
}

extension InputMgrView : KeyboardMgrProtocol {
    func switchToKeyboard(inputKeyboard: InputKeyboard) {
        print("switchKeyboard")
        show(inputKeyboard: inputKeyboard)
    }
    
    func backMainKeyboard(){
        print("backMainKeyboard")
        show(inputKeyboard: mainKeyboard)
    }

    func nextInputMode() {
        print("nextInputMode")
        if (inputController != nil) {
            self.inputController.advanceToNextInputMode()
        }
    }
    
    func hideKeyboard(){
        print("hideKeyboard")
        if (inputController != nil) {
            self.inputController.dismissKeyboard()
        }

    }
    
    func handleInputModeList(from view: UIView, with event: UIEvent) {
        print("handleInputModeList")
        if (inputController != nil) {
            self.inputController.handleInputModeList(from: view, with: event)
        }
    }
    
    func insertText(_ text: String) {
        print("insertText " + text)
        
        if (inputController != nil) {
            self.inputController.textDocumentProxy.insertText(text)
        }
    }
    
    func setMarkedText(text: String) {
        print("setMarkedText " + text)
        if (inputController != nil) {
            if #available(iOSApplicationExtension 13.0, *) {
                // self.inputController.textDocumentProxy.setMarkedText(text, selectedRange: NSMakeRange (text.count, 0))
            } else {
                // Fallback on earlier versions
            }
        }
    }
    
    func deleteBackward() {
        print("deleteBackward")
        if (inputController != nil) {
            self.inputController.textDocumentProxy.deleteBackward()
        }
    }
    
    func getReturnKeyString() -> String {
        var ret:String = "Nil"

        if (inputController != nil) {
            switch(self.inputController.textDocumentProxy.returnKeyType){

            case .none:
                ret = "None"
            case .some(.default):
                ret = "换行"
            case .some(.go):
                ret = "Go"
            case .some(.google):
                ret = "Google"
            case .some(.join):
                ret = "Join"
            case .some(.next):
                ret = "Next"
            case .some(.route):
                ret = "Route"
            case .some(.search):
                ret = "搜索"
            case .some(.send):
                ret = "发送"
            case .some(.yahoo):
                ret = "Yahoo"
            case .some(.done):
                ret = "Done"
            case .some(.emergencyCall):
                ret = "emergencyCall"
            case .some(.continue):
                ret = "continue"
            case .some(_):
                ret = "Unknown"
            }
        }
//        print("getReturnKeyString " + ret)

        return ret
    }
}
