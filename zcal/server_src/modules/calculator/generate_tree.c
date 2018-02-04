#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"operator.h"
#include"analyze_unit.h"


/* 参数：树，插入结点的运算级别，单双目
 * 返回：返回插入位置的父结点，如果是根，则返回NULL
 * 出错，程序将自动退出 */
Unit *tree_insert_pos(Unit *tree, int level, int num);
void bianli(Unit *tree);

Unit * generate_tree(Ustack *pus){

	if(pus == NULL)
		return NULL;

	Unit *root;
	Unit *u_operator;
	Unit *insert_father;
	root = popUstack(pus);
	
	while(u_operator = popUstack(pus) ){

		if( u_operator->num_flag == 1){
			
			insert_father = tree_insert_pos(root, u_operator->level, 1);
			if(insert_father == NULL){
				u_operator->right = root;
				u_operator->left = NULL;
				root = u_operator;
			}else{
				u_operator->right = insert_father->left;
				u_operator->left = NULL;
				insert_father->left = u_operator;
			}

		}else{
		
			insert_father = tree_insert_pos(root, u_operator->level, 2);
		
			if(insert_father == NULL){
				u_operator->right = root;
				u_operator->left = popUstack(pus);
				root = u_operator;
			}else{
				u_operator->right = insert_father->left;
				u_operator->left = popUstack(pus);
				insert_father->left = u_operator;
		
			}
		}
	}
	
	destroyUstack(pus);

	return root;
}

/* 用来遍历树，debug */
void bianli(Unit *tree){
	if(!tree)
		return;
	bianli(tree->left);
	printf("%.*s ",tree->len,tree->str);
	bianli(tree->right);
}

Unit *tree_insert_pos(Unit *tree, int level, int num){


	Unit *insert_pos = tree;
	Unit *insert_father = NULL;
	while(insert_pos){
		if( num == 2  &&  insert_pos->level < level ||
				level == 1 && insert_pos->num_flag == 1)
			return insert_father;
		if(num == 1 &&  insert_pos->level <= level)
			return insert_father;
		insert_father = insert_pos;
		insert_pos = insert_pos->left;
	}
	
	
	/* 出现未知错误，退出程序 */
	exit(11);
}
