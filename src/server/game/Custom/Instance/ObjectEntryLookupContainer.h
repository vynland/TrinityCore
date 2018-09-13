#ifndef DEF_OBJECTENTRYLOOKUPCONTAINER_H
#define DEF_OBJECTENTRYLOOKUPCONTAINER_H

template<typename TEntryEnumType, typename TStorageType>
class ObjectEntryLookupContainer
{
    static_assert(std::is_enum<TEntryEnumType>::value, "TEntryEnumType must be an enumeration type.");

public:
    ObjectEntryLookupContainer() { }

    bool ContainsKey(TEntryEnumType entry)
    {
        return InternalMap.find(entry) != InternalMap.end();
    }

    bool Insert(TEntryEnumType entry, TStorageType storageValue)
    {
        //TODO: Do some checking for if it's already present.
        InternalMap.emplace(std::make_pair(entry, storageValue));
        return true;
    }

    TStorageType& FindByEntry(TEntryEnumType entry)
    {
        return InternalMap.at(entry);
    }
private:
    std::map<TEntryEnumType, TStorageType> InternalMap;
};

#endif
