#ifndef avl_treet_h
#define avl_treet_h

template <typename T>
class Tree;

template <typename T>
class TreeNode {
	
public:	
	signed char balance;
	
	TreeNode<T>* parent;
	TreeNode<T>* left;
	TreeNode<T>* right;	

	TreeNode() {
		balance = 0;
		parent = 0;
	}

	~TreeNode() {
		remove();
	}
	
	TreeNode<T>* successor() {
	  TreeNode<T>* Result = right;
	  if(Result) {
	    while(Result->left) Result = Result->left;
	  } else {
	    Result = this;
	    while(Result->parent && Result->parent->right == Result) {
	      Result = Result->parent;
	    }
	    Result = Result->parent;
	  }
	  return Result;
	}

	TreeNode<T>* precessor() {
	  TreeNode<T>* Result = left;
	  if(Result) {
	    while(Result->right) Result = Result->right;
	  } else {
	    Result = this;
	    while(Result->parent && Result->parent->left == Result) {
	      Result = Result->parent;
	    }
	    Result = Result->parent;
	  }
	  return Result;
	}
	
	Tree<T>* getTree() {
	  if(!parent) {
	  	return 0;
	  } else {
	    TreeNode<T>* baseNode = this;
	    while(baseNode->parent) baseNode = baseNode->parent;	    
        int s = (char*)&(((Tree<T>*)1)->base) - (char*)1;
	    return (Tree<T>*)((char*)baseNode - s);
	  }
	}
	
	bool isInTree() {
		return parent;
	}

	void remove() {
	  if(parent) {
	    Tree<T>* t = getTree();
	    t->remove(this);
	  }
	}
	
};

template <typename T>
class Tree {
	
typedef signed char (*CompareData)(const T&, const T&);
typedef signed char (*CompareKey)(const void*, const T&);

private:
	unsigned int shift;
	TreeNode<T>* root;
	CompareData compare;
	int count;
	
  TreeNode<T>* findInsertPos(T* Data, CompareData Comp = 0) {
	  if(!Comp) Comp = compare;
	  TreeNode<T>* Result = root;
	  while(Result) {
	    signed char c = Comp(*Data, *get(Result));
	    if(c < 0) {
	      if(Result->left) {
	        Result = Result->left;
	      } else {
	      	return Result;
		  }
	    } else {
	      if(Result->right)
	        Result = Result->right;
	      else
	        return Result;
	    }
	  }
	  return Result;
	}
	
	void switchPositionWithSuccessor(TreeNode<T>* ANode, TreeNode<T>* ASuccessor) {	
	  signed char OldBalance = ANode->balance;
	  ANode->balance = ASuccessor->balance;
	  ASuccessor->balance = OldBalance;
	
	  TreeNode<T>* OldParent = ANode->parent;
	  TreeNode<T>* OldLeft = ANode->left;
	  TreeNode<T>* OldRight = ANode->right;
	  TreeNode<T>* OldSuccParent = ASuccessor->parent;
	  TreeNode<T>* OldSuccLeft = ASuccessor->left;
	  TreeNode<T>* OldSuccRight = ASuccessor->right;
	
	  if(OldParent != &base) {
	    if(OldParent->left == ANode)
	      OldParent->left = ASuccessor;
	    else
	      OldParent->right=ASuccessor;
	  } else
	    setRoot(ASuccessor);
	  ASuccessor->parent = OldParent;
	
	  if(OldSuccParent != ANode) {
	    if(OldSuccParent->left == ASuccessor)
	      OldSuccParent->left = ANode;
	    else
	      OldSuccParent->right = ANode;
	    ASuccessor->right = OldRight;
	    ANode->parent = OldSuccParent;
	    if(OldRight)
	      OldRight->parent = ASuccessor;
	  } else {
	    ASuccessor->right = ANode;
	    ANode->parent = ASuccessor;
	  }
	  ANode->left = OldSuccLeft;
	  if(OldSuccLeft) OldSuccLeft->parent = ANode;
	  ANode->right = OldSuccRight;
	  if(OldSuccRight) OldSuccRight->parent = ANode;
	  ASuccessor->left = OldLeft;
	  if(OldLeft) OldLeft->parent = ASuccessor;
	}	
	
	void balanceAfterInsert(TreeNode<T>* ANode) {
	  TreeNode<T>* OldParent = ANode->parent;
	  while(OldParent != &base) {
	    if(OldParent->left == ANode) {
	      OldParent->balance--;
	      if(OldParent->balance == 0) return;
	      if(OldParent->balance == -1) {
	        ANode = OldParent;
	        OldParent = ANode->parent;
	        continue;
	      }
	      if(ANode->balance == -1){
	        rotateRight(OldParent);
	        ANode->balance = 0;
	        OldParent->balance = 0;
	      } else {
	        TreeNode<T>* OldRight = ANode->right;
	        rotateLeft(ANode);
	        rotateRight(OldParent);
	        if(OldRight->balance <= 0)
	          ANode->balance = 0;
	        else
	          ANode->balance = -1;
	        if (OldRight->balance == -1)
	          OldParent->balance = 1;
	        else
	          OldParent->balance = 0;
	        OldRight->balance = 0;
	      }
	      return;
	    } else {
	      OldParent->balance++;
	      if(OldParent->balance == 0) return;
	      if(OldParent->balance == 1) {
	        ANode = OldParent;
	        OldParent = ANode->parent;
	        continue;
	      }
	      if(ANode->balance == 1) {
	        rotateLeft(OldParent);
	        ANode->balance = 0;
	        OldParent->balance = 0;
	      } else {
	        TreeNode<T>* OldLeft = ANode->left;
	        rotateRight(ANode);
	        rotateLeft(OldParent);
	        if(OldLeft->balance >= 0)
	          ANode->balance = 0;
	        else
	          ANode->balance = 1;
	        if(OldLeft->balance == 1)
	          OldParent->balance = -1;
	        else
	          OldParent->balance = 0;
	        OldLeft->balance = 0;
	      }
	      return;
	    }
	  }		
	}
	
	void balanceAfterDelete(TreeNode<T>* ANode) {	
		while(ANode) {		
		    if(ANode->balance == 1 || ANode->balance == -1) return;
		    TreeNode<T>* OldParent = ANode->parent;
		    if(ANode->balance == 0) {
		      if(OldParent == &base) return;
		      if(OldParent->left == ANode)
		        OldParent->balance++;
		      else
		        OldParent->balance--;
		      ANode = OldParent;
		    } else if(ANode->balance == 2) {
		      TreeNode<T>* OldRight = ANode->right;
		      if(OldRight->balance >= 0) {
		        rotateLeft(ANode);
		        ANode->balance = 1 - OldRight->balance;
		        OldRight->balance--;
		        ANode = OldRight;
		      } else {
		        TreeNode<T>* OldRightLeft = OldRight->left;
		        rotateRight(OldRight);
		        rotateLeft(ANode);
		        if(OldRightLeft->balance <= 0)
		          ANode->balance = 0;
		        else
		          ANode->balance = -1;
		        if(OldRightLeft->balance >= 0)
		          OldRight->balance=0;
		        else
		          OldRight->balance = 1;
		        OldRightLeft->balance = 0;
		        ANode = OldRightLeft;
		      }
		    } else {
		      TreeNode<T>* OldLeft = ANode->left;
		      if(OldLeft->balance <= 0) {
		        rotateRight(ANode);
		        ANode->balance = (-1 - OldLeft->balance);
		        OldLeft->balance++;
		        ANode = OldLeft;
		      } else {
		        TreeNode<T>* OldLeftRight = OldLeft->right;
		        rotateLeft(OldLeft);
		        rotateRight(ANode);
		        if(OldLeftRight->balance >= 0)
		          ANode->balance = 0;
		        else
		          ANode->balance = 1;
		        if(OldLeftRight->balance <=0)
		          OldLeft->balance = 0;
		        else
		          OldLeft->balance = -1;
		        OldLeftRight->balance = 0;
		        ANode = OldLeftRight;
		      }
		    }	
		}
	}
	
	void rotateRight(TreeNode<T>* ANode) {
	  TreeNode<T>* OldLeft = ANode->left;
	  TreeNode<T>* OldLeftRight = OldLeft->right;
	  TreeNode<T>* AParent = ANode->parent;
	  if(AParent != &base) {
	    if(AParent->left == ANode)
	      AParent->left = OldLeft;
	    else
	      AParent->right = OldLeft;
	  } else
	    setRoot(OldLeft);
	  OldLeft->parent = AParent;
	  ANode->parent = OldLeft;
	  ANode->left = OldLeftRight;
	  if(OldLeftRight) {
	    OldLeftRight->parent = ANode;
	  }
	  OldLeft->right = ANode;	
    }
    
	void rotateLeft(TreeNode<T>* ANode) {		
	  TreeNode<T>* OldRight = ANode->right;
	  TreeNode<T>* OldRightLeft = OldRight->left;
	  TreeNode<T>* AParent = ANode->parent;
	  if(AParent != &base) {
	    if(AParent->left == ANode)
	      AParent->left = OldRight;
	    else
	      AParent->right = OldRight;
	  } else
	    setRoot(OldRight);
	  OldRight->parent = AParent;
	  ANode->parent = OldRight;
	  ANode->right = OldRightLeft;
	  if(OldRightLeft) {
	    OldRightLeft->parent = ANode;
	  }
	  OldRight->left = ANode;
	}
	
  void deleteNode(TreeNode<T>* ANode) {
    if(ANode->left) deleteNode(ANode->left);
    if(ANode->right) deleteNode(ANode->right);
    ANode->parent = 0;
    ANode->left = 0;
    ANode->right = 0;
    ANode->balance = 0;
  }			
	  
public:
	
  TreeNode<T> base;	
	
	Tree(int shift, CompareData compare) {
		this->shift = shift;
		this->compare = compare;
	}
	
	Tree(void* shift, CompareData compare) {
		this->shift = (unsigned int)shift;
		this->compare = compare;
	}

	~Tree() {
		clear();
	}  

  T* get(TreeNode<T>* node) {
  	if(node)
    	return (T*)((char*)node - shift);
    else
    	return 0;
  }

  TreeNode<T>* get(T* node) {
  	if(node)
    	return (TreeNode<T>*)((char*)node + shift);
    else
    	return 0;
  }
  
  TreeNode<T>* getLeftmostNode() {
	  TreeNode<T>* Result = root;
	  if(Result) {
	  	  while(Result->left) Result = Result->left;
	  }	  
	  return Result;  	
  }

  TreeNode<T>* geRightmostNode() {
	  TreeNode<T>* Result = root;
	  if(Result) {
	  	  while(Result->right) Result = Result->right;
	  }	  
	  return Result;  	
  }
  
  void setRoot(TreeNode<T>* root) {
	  this->root = root;
	  base.right = root;
	  base.left = root;
	  if(root) this->root->parent = &base;  	
  }
  
	void add(TreeNode<T>* ANode, CompareData Comp = 0) {
	  if(!Comp) Comp = compare;
	  if(ANode->parent) ANode->remove();
	  ANode->left =  0;
	  ANode->right =  0;
	  ANode->balance = 0;
	  T* Data = get(ANode);
	  count++;
	  if(root) {
	    TreeNode<T>* InsertPos = findInsertPos(Data, Comp);
	    signed char InsertComp = Comp(*Data, *get(InsertPos));
	    ANode->parent = InsertPos;
	    if(InsertComp < 0)
	      InsertPos->left = ANode;
	    else
	      InsertPos->right = ANode;
	    balanceAfterInsert(ANode);
	  } else {
	    setRoot(ANode);
	  }
	}  
  
  void remove(TreeNode<T>* ANode) {  	
	  if(!ANode->parent) return;
	  if(ANode->left && ANode->right) {
	    switchPositionWithSuccessor(ANode, ANode->successor());
	  }
	  TreeNode<T>* OldParent = ANode->parent;
	  ANode->parent =  0;
	  TreeNode<T>* Child;
	  if(ANode->left)
	    Child = ANode->left;
	  else
	    Child = ANode->right;
	  if(Child)
	    Child->parent = OldParent;
	  if(OldParent != &base) {
	    if(OldParent->left == ANode) {
	      OldParent->left = Child;
	      OldParent->balance++;
	    } else {
	      OldParent->right = Child;
	      OldParent->balance--;
	    }
	    balanceAfterDelete(OldParent);
	  } else {
	    setRoot(Child);
	  }
	  count--;
  }
  
  void clear() {
	  if(root) deleteNode(root);
	  setRoot( 0);
	  count = 0;
  }  
  
  TreeNode<T>* findLeftmostGE(TreeNode<T>& node, CompareData Comp = 0) {
        if(!Comp) Comp = this->compare;
        TreeNode<T>* result = 0;
        TreeNode<T>* node2 = this->root;
        while(node2) {
        	int n = Comp(*get(node), *get(node2));
            if(n <= 0) {
                result = node2;
                node2 = node2->left;
            } else {
                node2 = node2->right;
            }
        }
        return result;
    }
	
  TreeNode<T>* findLeftmostGE(const void* key, CompareKey Comp) {
        TreeNode<T>* result = 0;
        TreeNode<T>* node2 = this->root;
        while(node2) {
        	int n = Comp(key, *get(node2));
            if(n <= 0) {
                result = node2;
                node2 = node2->left;
            } else {
                node2 = node2->right;
            }
        }
        return result;
    }  	  	

};

#endif
