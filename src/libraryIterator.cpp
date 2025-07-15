#include <libraryIterator.hpp>

LibraryIterator::typeReference LibraryIterator::operator*() const {
	return *mIter;
}
LibraryIterator::typePointer LibraryIterator::operator->() const {
	return &(*mIter);
};

LibraryIterator &LibraryIterator::operator++() {
	if (mIter + 1 == mMapIter->second->songs.end() &&
	    mMapIter._M_cur->_M_next() != mOriginalMap.end()) {
		++mMapIter;
		mIter = mMapIter->second->songs.begin();
	}
	++mIter;
	return *this;
}
LibraryIterator &LibraryIterator::operator++(int) {
	auto tmp = this;
	this->operator++();
	return *tmp;
}

bool LibraryIterator::operator==(const LibraryIterator &other) const {
	return other.mIter == mIter;
}
bool LibraryIterator::operator!=(const LibraryIterator &other) const {
	return !(other == *this);
}

LibraryIterator::LibraryIterator(
    map &originalMap,
    const std::optional<Album::ContainerType::iterator> currentIter)
    : mMapIter(originalMap.begin()), mOriginalMap(originalMap),
      mIter(mMapIter->second->songs.begin()),
      mEndIter(currentIter.value_or(mOriginalMap.end()->second->songs.end())) {}

LibraryIterator LibraryIterator::end() {
	LibraryIterator &it = *this;
	while ((++it).mIter != mOriginalMap.end()->second->songs.end())
		;
	return it;
}
