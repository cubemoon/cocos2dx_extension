#include "DialogLayer.h"

USING_NS_CC;
USING_NS_CC_EXT;


DialogLayer::DialogLayer():
    // m_pDefaultMenu(NULL),
	//    mTouchedMenu(false),
	m_bOffBackgoundColorBeforeEndDialogAction(false),
	m_pCurrentTouchMenu(NULL),
	m_pCurrentTouchEditBox(NULL),
	m_pCurrentTouchScrollView(NULL),
	m_pVisibleNode(NULL),
    mMenuItemArray(NULL),
	m_pOtherTouchCtl(NULL),
	m_nTouchCount(0),
	m_pBoardOnEnterAction(NULL),
	m_pBoardOnExitAction(NULL),
	m_pCallFuncN(NULL),
	m_pMainBoard(NULL)
{
}

DialogLayer::~DialogLayer()
{
}

bool DialogLayer::init()
{
    bool bRet = false;
    
    do {
        CC_BREAK_IF(!CCLayerColor::initWithColor(ccc4(0, 0, 0, 125)));
        
        onInitDialog();
        initMenu();

        bRet = true;
    } while (0);
    
    return bRet;
}

bool DialogLayer::initMenu()
{
    if (mMenuItemArray && mMenuItemArray->count() > 0) {
//        if (!m_pDefaultMenu) {
		// ����Ĭ�ϰ�ť
            CCMenu* pDefaultMenu = CCMenu::createWithArray(mMenuItemArray);
            pDefaultMenu->setPosition(CCPointZero);
            addChild(pDefaultMenu);

			// ����ǰ�˵��������
			m_vMenuArrays.push_back(pDefaultMenu);
 //       }
    }
    return true;
}

void DialogLayer::onEnter()
{
    CCLayerColor::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kCCMenuHandlerPriority - 1, true);

	// ���Ž�������
	if (m_pMainBoard && m_pBoardOnExitAction)
	{
		m_pMainBoard->stopAllActions();
		m_pMainBoard->runAction(m_pBoardOnEnterAction);
	}
}

void DialogLayer::onExit()
{
    CCLayerColor::onExit();
	// �Լ�ע��
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
}

bool DialogLayer::ccTouchBegan(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	m_nTouchCount++;

	m_pCurrentTouchMenu = NULL;
	// ѭ���ж��ڲ�ÿ����ť����Ӧ�����¼�����һ����Ӧ������ѭ��
	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu && (*iterMenu)->ccTouchBegan(pTouch, pEvent))
		{
			// mTouchedMenu = m_pMenu->ccTouchBegan(pTouch, pEvent);
			// ��¼ǰ�ػ�����Ϣ�İ�ť
			m_pCurrentTouchMenu = *iterMenu;
			return true;
		}
	}

	// ѭ���ж��Ƿ��������ؼ���Ӧ��Ϣ������editbox֮��
	m_pCurrentTouchEditBox = NULL;
	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox && (*iterEditBox)->isVisible() && (*iterEditBox)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchEditBox = *iterEditBox;
			return true;
		}
	}

	// ���ScrollView
	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if (*iterScrollView && (*iterScrollView)->isVisible() && (*iterScrollView)->ccTouchBegan(pTouch, pEvent))
		{
			m_pCurrentTouchScrollView = *iterScrollView;
			return true;
		}
	}

	m_pOtherTouchCtl = NULL;
	std::vector<CCLayer*>::iterator iterOtherCtl = m_vOtherCtlArray.begin();
	for (; iterOtherCtl != m_vOtherCtlArray.end(); ++iterOtherCtl)
	{
		if (*iterOtherCtl && (*iterOtherCtl)->isVisible() && (*iterOtherCtl)->ccTouchBegan(pTouch, pEvent))
		{
			m_pOtherTouchCtl = *iterOtherCtl;
			return true;
		}
	}

	// �ж��Ƿ�������ʾ���֮�������
//	if (m_pVisibleNode)
	if (m_pMainBoard)
	{
		CCPoint touchLocation = pTouch->getLocation();
		CCPoint local = m_pVisibleNode->convertToNodeSpace(touchLocation);
		// CCPoint local = m_pMainBoard->convertToNodeSpace(touchLocation);

		CCPoint nodePostion = m_pVisibleNode->getPosition();
		// CCPoint nodePostion = m_pMainBoard->getPosition();

		CCSize nodeContentSize = m_pVisibleNode->getContentSize();
		// CCSize nodeContentSize = m_pMainBoard->getContentSize();

		CCRect r = CCRectMake( nodePostion.x - nodeContentSize.width * m_obAnchorPoint.x,
			nodePostion.y - nodeContentSize.height * m_obAnchorPoint.y,
			nodeContentSize.width, nodeContentSize.height);
		r.origin = CCPointZero;

		// �����ǰ�㲻����������ڣ���ֱ���˳���ǰ����
		if (!r.containsPoint(local))
		{
			if (m_pCurrentTouchScrollView && m_pCurrentTouchScrollView->isDragging())
			{
				// �����ScrollView��ѡ�У����������Ϸ�״̬�����˳���ǰ�Ի���
				return true;
			}
			// �����Ի���
			endDialog();
		}	
	}

	// �̵���Ϣ
    return true;
}

void DialogLayer::ccTouchMoved(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu)
	{
        m_pCurrentTouchMenu->ccTouchMoved(pTouch, pEvent);
    }

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchMoved(pTouch, pEvent);
	}

	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchMoved(pTouch, pEvent);
	}
}

void DialogLayer::ccTouchEnded(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
//	CCLOG("DialogLayer::ccTouchEnded()");
	m_nTouchCount--;

    if (m_pCurrentTouchMenu) 
	{
        m_pCurrentTouchMenu->ccTouchEnded(pTouch, pEvent);
    }
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchEnded(pTouch, pEvent);
	}
 	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchEnded(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;

	if (m_pCurrentTouchScrollView)
	{
		CCLOG("m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);");
		m_pCurrentTouchScrollView->ccTouchEnded(pTouch, pEvent);
	}

	if (m_nTouchCount == 0)
	{
		m_pCurrentTouchScrollView = NULL;
	}
}

void DialogLayer::ccTouchCancelled(cocos2d::CCTouch *pTouch, cocos2d::CCEvent *pEvent)
{
	if (m_pCurrentTouchMenu) 
	{
		m_pCurrentTouchMenu->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchMenu = NULL;

	if (m_pCurrentTouchEditBox)
	{
		m_pCurrentTouchEditBox->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchEditBox = NULL;

	if (m_pOtherTouchCtl)
	{
		m_pOtherTouchCtl->ccTouchCancelled(pTouch, pEvent);
	}
	m_pOtherTouchCtl = NULL;
	
	if (m_pCurrentTouchScrollView)
	{
		m_pCurrentTouchScrollView->ccTouchCancelled(pTouch, pEvent);
	}
	m_pCurrentTouchScrollView = NULL;
}

void DialogLayer::pushMenuItem( cocos2d::CCMenuItem* pMenuItem )
{
	if (!mMenuItemArray) {
		mMenuItemArray = CCArray::create();
	}

	mMenuItemArray->addObject(pMenuItem);
}

void DialogLayer::pushMenu( cocos2d::CCMenu* pMenu )
{
	if (pMenu != NULL)
	{
		m_vMenuArrays.push_back(pMenu);
	}
}

void DialogLayer::pushOtherTouchedCtl( cocos2d::CCLayer* pOtherCtl )
{
	if (pOtherCtl)
	{
		m_vOtherCtlArray.push_back(pOtherCtl);
	}
}

void DialogLayer::SetVisibleBoard( CCNode* pNode )
{
	m_pVisibleNode = pNode;
}

void DialogLayer::pushEditBox( cocos2d::extension::CCEditBox* pEditBox )
{
	if (pEditBox != NULL)
	{
		m_vEditBoxArrays.push_back(pEditBox);
	}
}

/*
*	�����Ҫ���Ž�����������ʹ��addBoardChildWithAction�������趨��Ӧ�Ķ�������Ͷ���
*/
bool DialogLayer::endDialog(cocos2d::CCActionInterval* pAction /*= NULL*/, float fDuration /*= 1.0f*/)
{
	if (pAction)
	{
		m_pBoardOnExitAction = pAction;
	}

	if (m_pBoardOnExitAction && m_pMainBoard)
	{
		if (m_bOffBackgoundColorBeforeEndDialogAction)
		{
			setOpacity(0);
		}
		if (m_pCallFuncN)
		{
			m_pMainBoard->runAction(CCSequence::create(m_pBoardOnExitAction, m_pCallFuncN, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
		}
		else
		{
			// Ĭ�϶������ź��Ƴ�DialogLayer
			m_pMainBoard->runAction(CCSequence::create(m_pBoardOnExitAction, CCCallFunc::create(this, callfunc_selector(DialogLayer::ActionCallbackRemoveLayer)), NULL));
		}
	}
	else
	{
		// ����ж�����ִ�ж���û����ֱ��ɾ�������Ҳ����������ʵ��
		removeFromParentAndCleanup(true);
	}
	return true;
}

void DialogLayer::pushScrollView( cocos2d::extension::CCScrollView* pScrollView )
{
	if (pScrollView)
	{
		m_vScrollViewArrays.push_back(pScrollView);
	}
}

bool DialogLayer::popMenu( cocos2d::CCMenu* pMenu )
{
	if (!pMenu)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if (*iterMenu == pMenu)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popMenuByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCMenu*>::iterator iterMenu = m_vMenuArrays.begin();
	for (; iterMenu != m_vMenuArrays.end(); ++iterMenu)
	{
		if ((*iterMenu)->getTag() == nTag)
		{
			m_vMenuArrays.erase(iterMenu);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBoxByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if ((*iterEditBox)->getTag() == nTag)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popEditBox( CCEditBox* pEditBox )
{
	if (!pEditBox)
	{
		return false;
	}

	std::vector<CCEditBox*>::iterator iterEditBox = m_vEditBoxArrays.begin();
	for (; iterEditBox != m_vEditBoxArrays.end(); ++iterEditBox)
	{
		if (*iterEditBox == pEditBox)
		{
			m_vEditBoxArrays.erase(iterEditBox);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollView( CCScrollView* pScrollView )
{
	if (!pScrollView)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if (*iterScrollView == pScrollView)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popScrollViewByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCScrollView*>::iterator iterScrollView = m_vScrollViewArrays.begin();
	for (; iterScrollView != m_vScrollViewArrays.end(); ++iterScrollView)
	{
		if ((*iterScrollView)->getTag() == nTag)
		{
			m_vScrollViewArrays.erase(iterScrollView);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtl( CCLayer* pOtherCtl )
{
	if (!pOtherCtl)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if (*iterCtl == pOtherCtl)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}

bool DialogLayer::popOtherTouchedCtrlByTag( int nTag )
{
	if (nTag == -1)
	{
		return false;
	}

	std::vector<CCLayer*>::iterator iterCtl = m_vOtherCtlArray.begin();
	for (; iterCtl != m_vOtherCtlArray.end(); ++iterCtl)
	{
		if ((*iterCtl)->getTag() == nTag)
		{
			m_vOtherCtlArray.erase(iterCtl);
			return true;
		}
	}

	return false;
}

bool DialogLayer::addBoardChildWithAction( cocos2d::CCNode* pChildNode, 
	cocos2d::CCActionInterval* pOnEnterAction /*= NULL*/,
	cocos2d::CCActionInterval* pOnExitAction /*= NULL*/, 
	cocos2d::CCCallFuncN* pAfterExitActionCallbackFuncN /*= NULL*/ )
{
	if (pChildNode)
	{
		m_pMainBoard = pChildNode;
		// Ĭ�ϵ�ǰ����弴Ϊ�������
		m_pVisibleNode = m_pMainBoard;
		addChild(pChildNode);
	}

	m_pCallFuncN = pAfterExitActionCallbackFuncN;
	m_pCallFuncN->retain();
	m_pBoardOnEnterAction = pOnEnterAction;
	m_pBoardOnEnterAction->retain();
	m_pBoardOnExitAction = pOnExitAction;
	m_pBoardOnExitAction->retain();

	return true;
}

void DialogLayer::ActionCallbackRemoveLayer()
{
	removeFromParentAndCleanup(true);
}

void DialogLayer::SetBackgroundColorOffBeforeEndDialogAction( bool bOff )
{
	m_bOffBackgoundColorBeforeEndDialogAction = bOff;
}
