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

    TStorageType FindByEntry(TEntryEnumType entry) const
    {
        auto f = InternalMap.find(entry);
        if (f != InternalMap.end())
            return (*f).second;

        //TODO: Handle ObjectGuid case better somehow
        return TStorageType();
    }
private:
    std::map<TEntryEnumType, TStorageType> InternalMap;
};

#endif
