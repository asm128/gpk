#include "gpk_index_path.h"
#include "gpk_file.h"

usng nmsp gpk;

sttc gpk::err_t	pathRelative				(gpk::string & output, cnst gpk::SPathTree & tree, cnst gpk::SPathNode & node) {
	if_fail_fe(output.clear());
	cnst SPathNode		* hierarchy[128]					= {};
	u2_t				count							= 0;
	for(cnst SPathNode * current = &node; current; current = current->Parent) {
		if_true_fef(count >= size(hierarchy), "Path hierarchy exceeds SPathInfo: %u.", count);
		hierarchy[count++] = current;
	}
	while(count) {
		cnst SPathNode		& current						= *hierarchy[--count];
		if_true_fef(current.ObjectIndex >= tree.Name.size(), "Invalid path object index: %u.", current.ObjectIndex);
		if(output.size())
			if_fail_fe(output.push_back('/'));
		if_fail_fe(output.append(tree.Name[current.ObjectIndex]));
	}
	rtrn output.size();
}

sttc gpk::err_t	pathNodePath				(gpk::string & relativePath, gpk::string & output, cnst gpk::SPathTree & tree, cnst gpk::SPathNode & node) {
	if_fail_fe(::pathRelative(relativePath, tree, node));
	if_fail_fe(output.clear());
	rtrn pathNameCompose(tree.Root, relativePath, output);
}

gpk::err_t	gpk::pathNodePath			(gpk::string & output, cnst gpk::SPathTree & tree, cnst gpk::SPathNode & node) {
	gpk::string			relativePath						= {};
	rtrn ::pathNodePath(relativePath, output, tree, node);
}

sttc gpk::err_t	pathWordViews				(gpk::aobj<gpk::vcst_t> & upper, cnst gpk::SPathTree & tree, cnst gpk::SPathNode & node) {
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
	rtrn 0;
}

sttc bool	pathContentsText				(gpk::vcst_t contents) {
	for(u2_t iByte = 0; iByte < contents.size(); ++iByte) {
		cnst u0_t		value							= (u0_t)contents[iByte];
		if(0 == value || (value < ' ' && '\t' != value && '\n' != value && '\r' != value))
			rtrn false;
	}
	rtrn true;
}

sttc gpk::err_t	pathContentWordViews			(gpk::aobj<gpk::vcst_t> & upper, gpk::vcst_t contents) {
	for(u2_t begin = 0; begin < contents.size();) {
		while(begin < contents.size() && (u0_t)contents[begin] <= ' ')
			++begin;
		u2_t				end								= begin;
		while(end < contents.size() && (u0_t)contents[end] > ' ')
			++end;
		if(begin < end)
			if_fail_fe(upper.push_back({contents.begin() + begin, end - begin}));
		begin = end;
	}
	rtrn upper.size();
}

sttc gpk::err_t	pathIndexViews				(gpk::SPathIndexData & indexData, gpk::aobj<gpk::vcst_t> & upper, cnst gpk::SPathTree & tree, cnst gpk::SPathNode & node) {
	if_fail_fe(::pathNodePath(indexData.RelativePath, indexData.FilePath, tree, node));
	if(indexData.IndexPath) {
		if_fail_fe(::gpk::split({indexData.FilePath}, '/', upper));
		if_fail_fe(::pathWordViews(upper, tree, node));
	}
	if(node.Info->Folder) {
		if_fail_fe(indexData.FileContents.clear());
		rtrn 0;
	}
	if_fail_fe(fileToMemory(indexData.FilePath, indexData.FileContents));
	if(false == ::pathContentsText(indexData.FileContents))
		rtrn 0;
	rtrn ::pathContentWordViews(upper, indexData.FileContents);
}

gpk::err_t	gpk::pathWordList					(aobj<string> & itemWords, aobj<vcst_t> & upper, cnst SPathTree & tree, cnst SPathNode & node, view<vcst_c> exceptions) {
	if_fail_fe(upper.clear());
	if_fail_fe(::pathWordViews(upper, tree, node));
	rtrn gpk::getWordList(itemWords, upper, exceptions);
}

gpk::err_t	gpk::getWordDict	(SPathIndexData & indexData, aobj<SWordIndices> & wordSet, aobj<vcst_t> & reverseMap, cnst SPathTree & tree, view<vcst_c> exceptions) {
	if_true_fef(tree.Tree.size() != tree.Name.size(), "Path tree nodes:%u, names:%u.", tree.Tree.size(), tree.Name.size());
	if_fail_fe(reverseMap.resize(tree.Name.size()));
	rtrn gpk::getWordDict(indexData.WordData, wordSet, tree.Tree.size()
		, [&indexData, &reverseMap, &tree](u2_t iNode, u2_t & itemIndex, aobj<vcst_t> & itemViews) -> err_t {
			cnst pobj<SPathNode> & node = tree.Tree[iNode];
			if_true_fef(!node || !node->Info || node->ObjectIndex >= tree.Name.size(), "Invalid path node: %u.", iNode);
			itemIndex = node->ObjectIndex;
			reverseMap[itemIndex] = tree.Name[itemIndex];
			rtrn ::pathIndexViews(indexData, itemViews, tree, *node);
		}
		, exceptions);
}
