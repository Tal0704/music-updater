#include <library.hpp>

Iterator::typeReference Iterator::operator*() const { return *mCurrent; }
Iterator::typePointer Iterator::operator->() const { return &(*mCurrent); };

Iterator &Iterator::operator++() {
	++mCurrent;

	if (mCurrent == mCurrentAlbum->second->songs.end()) {
		mCurrentAlbum++;
		if (mCurrentAlbum == mOriginalMap.end()) {
			return *this;
		}
		mCurrent = mCurrentAlbum->second->songs.begin();
	}
	return *this;
}
Iterator &Iterator::operator++(int) {
	auto tmp = this;
	this->operator++();
	return *tmp;
}

bool Iterator::operator==(const Iterator &other) const {
	return other.mCurrent == mCurrent;
}
bool Iterator::operator!=(const Iterator &other) const {
	return !(other == *this);
}

Iterator::Iterator(
    AlbumsType &originalMap,
    const std::optional<Album::ContainerType::iterator> currentIter)
    : mCurrentAlbum(originalMap.begin()), mOriginalMap(originalMap) {
	mCurrent = currentIter.value_or(originalMap.begin()->second->songs.begin());
}

Iterator Iterator::end() {
	Iterator &it = *this;
	while ((++it).mCurrent != mOriginalMap.end()->second->songs.end())
		;
	return it;
}
