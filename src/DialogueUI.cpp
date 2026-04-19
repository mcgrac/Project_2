//#include "DialogueUI.h"
//#include "DialogueManager.h"
//#include <iostream>
//
//void DialogueUI::render()
//{
//    DialogueManager& manager = DialogueManager::getInstance();
//
//    if (!manager.isActive())
//        return;
//
//    DialogueNode* node = manager.getCurrentNode();
//
//    if (!node)
//        return;
//
//    std::cout << "-----------------------------" << std::endl;
//    std::cout << node->speaker << std::endl;
//    std::cout << node->text << std::endl;
//
//    if (!node->options.empty())
//    {
//        for (int i = 0; i < node->options.size(); i++)
//        {
//            std::cout << i << ": " << node->options[i].text << std::endl;
//        }
//    }
//
//    std::cout << "-----------------------------" << std::endl;
//}
//
//void DialogueUI::handleOptionClick(int optionIndex)
//{
//    DialogueManager::getInstance().chooseOption(optionIndex);
//}