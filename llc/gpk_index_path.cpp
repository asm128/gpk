#include "gpk_index_path.h"

gpk::err_t	gpk::pathWordList					(aobj<string> & itemWords, aobj<vcst_t> & upper, cnst SPathTree & tree, cnst SPathNode & node, view<vcst_c> exceptions) {
	if_fail_fe(upper.clear());
	for(cnst SPathNode * parent = node.Parent; parent; parent = parent->Parent)
		if_fail_fe(split(tree.Name[parent->ObjectIndex], ' ', upper));
	cnst vcst_t			fileName							= tree.Name[node.ObjectIndex];
	if_fail_fe(split(fileName, ' ', upper));
	vcst_t				title								= fileName;
	if(false == node.Info->Folder) {
		if_fail_fe(pathStem(fileName, title));
		if(title.size() < fileName.size()) {
			vcst_t				extension;
			if_fail_fe(fileName.slice(extension, title.size() + 1));
			if_fail_fe(split(extension, ' ', upper));
		}
	}
	if_fail_fe(split(title, ' ', upper));
	if_fail_fe(upper.push_back(node.Info->Folder ? vcst_t{"folder"} : vcst_t{"file"}));
	rtrn gpk::wordListNormalize(itemWords, upper, exceptions);
}

gpk::err_t	gpk::getWordDict	(aobj<SWordIndices> & wordSet, aobj<vcst_t> & reverseMap, cnst SPathTree & tree, aobj<string> & cacheItemWords, view<vcst_c> exceptions) {
	if_true_fef(tree.Tree.size() != tree.Name.size(), "Path tree nodes:%u, names:%u.", tree.Tree.size(), tree.Name.size());
	if_fail_fe(reverseMap.resize(tree.Name.size()));
	aobj<vcst_t>			cacheItemViews						= {};
	for(u2_t iNode = 0; iNode < tree.Tree.size(); ++iNode) {
		cnst pobj<SPathNode> & node = tree.Tree[iNode];
		if_true_fef(!node || !node->Info || node->ObjectIndex >= tree.Name.size(), "Invalid path node: %u.", iNode);
		reverseMap[node->ObjectIndex] = tree.Name[node->ObjectIndex];
		if_fail_fe(pathWordList(cacheItemWords, cacheItemViews, tree, *node, exceptions));
		if_fail_fe(wordDictAppend(wordSet, node->ObjectIndex, cacheItemWords));
	}
	rtrn 0;
}
