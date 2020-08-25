#ifndef COMPILEDRESOURCEDIRECTORY_H
#define COMPILEDRESOURCEDIRECTORY_H

struct CompiledResourceDirectoryEntry {
    char *name;
    const unsigned char *data;
    int length;
};

#endif // COMPILEDRESOURCEDIRECTORY_H
