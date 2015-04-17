/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 23, 2015
 * Updated: March 31, 2015
 */

#ifndef _FABRIC_INTERNAL_H__
#define _FABRIC_INTERNAL_H__

#ifndef MIN_PAGE_SIZE
#define MIN_PAGE_SIZE 65536
#endif
#ifndef INDEX_PAGE_SIZE
#define INDEX_PAGE_SIZE 65536
#endif

/**
 * Library for managing byte order on various systems
 */
#include "../ByteOrder/byteorder.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * Types used by Fabric
 * TODO: test for system float and double sizes
 */
typedef float float32_t;
typedef double float64_t;
typedef uint32_t error_t;


/**
 * Database storage sizes
 */
#define FABRIC_CLASS_STORAGE_SIZE 21
#define FABRIC_LABEL_STORAGE_SIZE 8

/**
 * Ids of preset indices
 */
#define FABRIC_CLASS_INDEX 1
#define FABRIC_LABEL_INDEX 2
#define FABRIC_EDGE_INDEX 3


/**
 * Temporary macros
 */
#ifndef _FABRIC_TEMP_MACROS__
#  define _FABRIC_TEMP_MACROS__
#  define Fabric_strncpy(dest, src, num) strncpy(dest, src, num)
#  define text_t char*
#endif

/**
 * Text constants
 */
#ifndef FABRIC_TEXT_BLOCK_SIZE
#  define FABRIC_TEXT_BLOCK_SIZE 32
#endif

/**
 * Booleans
 */
#ifndef FALSE
#  define FALSE 0
#endif
#ifndef TRUE
#  define TRUE 1
#endif
#ifndef bool_t
#  define bool_t uint8_t
#endif


/**
 * Entity structs
 */
struct Graph;
typedef struct Graph Graph;
struct Class;
typedef struct Class Class;
struct Label;
typedef struct Label Label;
struct Vertex;
typedef struct Vertex Vertex;
struct Edge;
typedef struct Edge Edge;
struct Property;
typedef struct Property Property;
struct Text;
typedef struct Text Text;
struct Index;
typedef struct Index Index;
struct ClassIndex;
typedef struct ClassIndex ClassIndex;

/**
 * Storage manager structs
 */
struct ClassStore;
typedef struct ClassStore ClassStore;
struct LabelStore;
typedef struct LabelStore LabelStore;
struct VertexStore;
typedef struct VertexStore VertexStore;
struct EdgeStore;
typedef struct EdgeStore EdgeStore;
struct PropertyStore;
typedef struct PropertyStore PropertyStore;
struct TextStore;
typedef struct TextStore TextStore;
struct IndexStore;
typedef struct IndexStore IndexStore;

/**
 * Collection types
 */
struct DynamicList;
typedef struct DynamicList DynamicList;
struct IdSet;
typedef struct IdSet IdSet;
struct EntityMap;
typedef struct EntityMap EntityMap;

/**
 * Id size typedefs
 */
typedef uint16_t classid_t;
typedef uint32_t labelid_t;
typedef uint32_t vertexid_t;
typedef uint32_t edgeid_t;
typedef uint32_t propertyid_t;
typedef uint32_t textid_t;
typedef uint16_t indexid_t;


/**
 * Memory functions
 */
int Fabric_meminit();
void *Fabric_memalloc(size_t size);
void Fabric_memfree(void* ptr, size_t size);
size_t Fabric_memused();
int Fabric_memerrno();

/**
 * Graph write methods
 */
void Fabric_Graph_write_bytes (Graph *self, uint8_t *bytes, int num_bytes, long offset);
void Fabric_Graph_write_uint32 (Graph *self, uint32_t value, long offset);

/**
 * Graph read methods
 */
void Fabric_Graph_read_bytes (Graph *self, uint8_t *destination, int num_bytes, long offset);
uint32_t Fabric_Graph_read_uint32 (Graph *self, long offset);
uint16_t Fabric_Graph_read_uint16 (Graph *self, long offset);

/**
 * Graph retrieval methods
 */
Graph* Fabric_ClassStore_get_graph (ClassStore *self);
Graph* Fabric_LabelStore_get_graph (LabelStore *self);
Graph* Fabric_VertexStore_get_graph (VertexStore *self);
Graph* Fabric_EdgeStore_get_graph (EdgeStore *self);
Graph* Fabric_PropertyStore_get_graph (PropertyStore *self);
Graph* Fabric_TextStore_get_graph (TextStore *self);
Graph* Fabric_IndexStore_get_graph (IndexStore *self);

/**
 * Store retrieval methods
 */
ClassStore *Fabric_Graph_get_class_store(Graph *self);
LabelStore *Fabric_Graph_get_label_store(Graph *self);
VertexStore *Fabric_Graph_get_vertex_store(Graph *self);
EdgeStore *Fabric_Graph_get_edge_store(Graph *self);
PropertyStore *Fabric_Graph_get_property_store(Graph *self);
TextStore *Fabric_Graph_get_text_store(Graph *self);
IndexStore *Fabric_Graph_get_index_store(Graph *self);

/**
 * Graph file offsets
 */
uint32_t Fabric_Graph_get_class_store_offset(Graph* self);
uint32_t Fabric_Graph_get_label_store_offset(Graph* self);
uint32_t Fabric_Graph_get_vertex_store_offset(Graph* self);
uint32_t Fabric_Graph_get_edge_store_offset(Graph* self);
uint32_t Fabric_Graph_get_property_store_offset(Graph* self);
uint32_t Fabric_Graph_get_text_store_offset(Graph* self);
uint32_t Fabric_Graph_get_index_store_offset(Graph* self);

/**
 * ClassStore methods
 */
error_t Fabric_ClassStore_init(ClassStore *self);
error_t Fabric_ClassStore_flush(ClassStore *self);
Class *Fabric_ClassStore_get_class(ClassStore *self, classid_t class_id, error_t *status);
Class *Fabric_ClassStore_get_class_by_name(ClassStore *self, text_t name, error_t *status);
Class *Fabric_ClassStore_create_class(
    ClassStore *self,
    Class *extends,
    text_t name,
    bool_t is_abstract,
    error_t *status);
error_t Fabric_ClassStore_delete_class(ClassStore *self, Class *c);

/**
 * LabelStore methods
 */
Label *Fabric_LabelStore_get_label(LabelStore *self, labelid_t label_id, error_t *status);
labelid_t Fabric_LabelStore_add_label(LabelStore *self, text_t name, error_t *status);
error_t Fabric_LabelStore_remove_label(LabelStore *self, labelid_t label_id);

/**
 * VertexStore methods
 */
Vertex *Fabric_VertexStore_get_vertex(VertexStore *self, edgeid_t edge_id, error_t *status);

/**
 * EdgeStore methods
 */
Edge *Fabric_EdgeStore_get_edge(EdgeStore *self, edgeid_t edge_id, error_t *status);

/**
 * PropertyStore methods
 */
Property *Fabric_PropertyStore_get_property(PropertyStore *self, propertyid_t property_id, error_t *status);

/**
 * TextStore methods
 */
Text *Fabric_TextStore_get_text(TextStore *self, textid_t text_id, error_t *status);
textid_t Fabric_TextStore_create_text(TextStore *self, text_t value, error_t *status);

/**
 * IndexStore methods
 */
Index *Fabric_IndexStore_get_index(IndexStore *self, indexid_t index_id, error_t *status);
ClassIndex *Fabric_IndexStore_get_class_index(IndexStore *self, error_t *status);
error_t Fabric_IndexStore_add_class_to_index(IndexStore *self, Class *class);
error_t Fabric_IndexStore_add_class_to_index_if_not_exists(IndexStore *self, Class *class);
error_t Fabric_IndexStore_remove_class_from_index(IndexStore *self, Class *class);
indexid_t Fabric_IndexStore_create_id_index(IndexStore *self, classid_t class_id, error_t *status);
error_t Fabric_IndexStore_delete_id_index(IndexStore *self, indexid_t index_id);

/**
 * Class methods
 */
Class *Fabric_Class_new(classid_t id, error_t *status);
error_t Fabric_Class_init(Class *self, uint8_t *data);
void Fabric_Class_destroy(Class *self);
void Fabric_Class_load_bytes(Class *self, uint8_t *dest);
classid_t Fabric_Class_get_id(Class *self);
void Fabric_Class_set_id(Class *self, classid_t id);
labelid_t Fabric_Class_get_label_id(Class *self);
void Fabric_Class_set_label_id(Class *self, labelid_t label_id);
bool_t Fabric_Class_is_in_use(Class *self);
Label *Fabric_Class_get_label(Class *self, Graph *graph, error_t *status);
classid_t Fabric_Class_get_parent_class_id(Class *self);
void Fabric_Class_set_parent_class_id(Class *self, classid_t parent_id);
Class *Fabric_Class_get_parent_class(Class *self, Graph *graph, error_t *status);
bool_t Fabric_Class_has_child_classes(Class *self);
classid_t Fabric_Class_get_first_child_class_id(Class *self);
void Fabric_Class_set_first_child_class_id(Class *self, classid_t child_class_id);
Class *Fabric_Class_get_first_child_class(Class *self, Graph *graph, error_t *status);
bool_t Fabric_Class_has_next_child_class(Class *self);
classid_t Fabric_Class_get_next_child_class_id(Class *self);
void Fabric_Class_set_next_child_class_id(Class *self, classid_t next_child_id);
Class *Fabric_Class_get_next_child_class(Class *self, Graph *graph, error_t *status);
error_t Fabric_Class_load_descendent_classes(Class *self, Graph *graph, DynamicList *list, int depth);
DynamicList *Fabric_Class_get_child_classes(Class *self, Graph *graph, error_t *status);
DynamicList *Fabric_Class_get_descendent_classes(Class *self, Graph *graph, error_t *status);
indexid_t Fabric_Class_get_first_index_id(Class *self);
void Fabric_Class_set_first_index_id(Class *self, indexid_t index_id);
Index *Fabric_Class_get_first_index(Class *self, Graph *graph, error_t *status);
uint32_t Fabric_Class_get_count(Class *self);
void Fabric_Class_set_count(Class *self, uint32_t count);
bool_t Fabric_Class_has_members(Class *self);
uint32_t Fabric_Class_get_total_count(Class *self, Graph *graph, error_t *status);
bool_t Fabric_Class_is_abstract(Class *self);
void Fabric_Class_set_is_abstract(Class *self, bool_t is_abstract);
uint32_t Fabric_Class_increment(Class *self);
void Fabric_Class_set_incrementer(Class *self, uint32_t value);

/**
 * Label methods
 */
labelid_t Fabric_Label_get_id(Label *self);
void Fabric_Label_set_id(Label *self, labelid_t id);
Label* Fabric_Label_new(labelid_t id, error_t *status);
void Fabric_Label_destroy(Label *self);
error_t Fabric_Label_init(Label *self, uint8_t *data);
textid_t Fabric_Label_get_text_id(Label *self);
void Fabric_Label_set_text_id(Label *self, textid_t text_id);
Text *Fabric_Label_get_text(Label *self, Graph *graph, error_t *status);
uint32_t Fabric_Label_get_refs(Label *self);
void Fabric_Label_set_refs(Label *self, uint32_t refs);
bool_t Fabric_Label_has_refs(Label *self);
void Fabric_Label_add_ref(Label *self);
void Fabric_Label_remove_ref(Label *self);

/**
 * Vertex Methods
 */
vertexid_t Fabric_Vertex_get_id(Vertex *self);
void Fabric_Vertex_set_id(Vertex *self, vertexid_t id);
error_t Fabric_Vertex_init(Vertex *self, uint8_t *data);
classid_t Fabric_Vertex_get_class_id(Vertex *self);
Class *Fabric_Vertex_get_class(Vertex *self, Graph *graph, error_t *status);
edgeid_t Fabric_Vertex_get_first_out_edge_id(Vertex *self);
bool_t Fabric_Vertex_has_out_edges(Vertex *self);
Edge *Fabric_Vertex_get_first_out_edge(Vertex *self, Graph *graph, error_t *status);
edgeid_t Fabric_Vertex_get_first_in_edge_id(Vertex *self);
bool_t Fabric_Vertex_has_in_edges(Vertex *self);
Edge *Fabric_Vertex_get_first_in_edge(Vertex *self, Graph *graph, error_t *status);
propertyid_t Fabric_Vertex_get_first_property_id(Vertex *self);
Property *Fabric_Vertex_get_first_property(Vertex *self, Graph *graph, error_t *status);
bool_t Fabric_Vertex_has_properties(Vertex *self);

/**
 * Edge methods
 */
labelid_t Fabric_Edge_get_label_id(Edge *self);
Label *Fabric_Edge_get_label(Edge *self, Graph *graph, error_t *status);
vertexid_t Fabric_Edge_get_from_vertex_id(Edge *self);
Vertex *Fabric_Edge_get_from_vertex(Edge *self, Graph *graph, error_t *status);
vertexid_t Fabric_Edge_get_to_vertex_id(Edge *self);
Vertex *Fabric_Edge_get_to_vertex(Edge *self, Graph *graph, error_t *status);
edgeid_t Fabric_Edge_get_next_out_edge_id(Edge *self);
Edge *Fabric_Edge_get_next_out_edge(Edge *self, Graph *graph, error_t *status);
bool_t Fabric_Edge_has_next_out_edge(Edge *self);
edgeid_t Fabric_Edge_get_next_in_edge_id(Edge *self);
Edge *Fabric_Edge_get_next_in_edge(Edge *self, Graph *graph, error_t *status);
bool_t Fabric_Edge_has_next_in_edge(Edge *self);
propertyid_t Fabric_Edge_get_first_property_id(Edge *self);
Property *Fabric_Edge_get_first_property(Edge *self, Graph *graph, error_t *status);
bool_t Fabric_Edge_has_properties(Edge *self);

/**
 * Property methods
 */
propertyid_t Fabric_Property_get_id(Property *self);
void Fabric_Property_set_id(Property *self, propertyid_t id);
error_t Fabric_Property_init(Property *self, uint8_t *data);
labelid_t Fabric_Property_get_label_id(Property *self);
Label *Fabric_Property_get_label(Property *self, Graph *graph, error_t *status);
propertyid_t Fabric_Property_get_next_property_id(Property *self);
Property *Fabric_Property_get_next_property(Property *self, Graph *graph, error_t *status);
bool_t Fabric_Property_has_next_property(Property *self);
uint8_t Fabric_Property_get_type(Property *self);
void Fabric_Property_set_type(Property *self, uint8_t type);
uint8_t* Fabric_Property_get_data(Property *self);
int64_t Fabric_Property_get_integer_value(Property *self);
void Fabric_Property_set_integer_value(Property *self, int64_t value);
float64_t Fabric_Property_get_real_value(Property *self);
void Fabric_Property_set_real_value(Property *self, float64_t value);
bool_t Fabric_Property_is_boolean(Property *self);
bool_t Fabric_Property_get_boolean_value(Property *self);
bool_t Fabric_Property_is_text(Property *self);
bool_t Fabric_Property_is_short_text(Property *self);
int Fabric_Property_get_short_text_length(Property *self);
void Fabric_Property_get_short_text(Property *self, text_t dest);
void Fabric_Property_set_short_text(Property *self, text_t source);
textid_t Fabric_Property_get_text_value_id(Property *self);
void Fabric_Property_set_text_value_id(Property *self, textid_t text_id);

/**
 * Text methods
 */
textid_t Fabric_Text_get_id(Text *self);
void Fabric_Text_set_id(Text *self, textid_t id);
error_t Fabric_Text_init(Text *self, uint8_t *data);
uint32_t Fabric_Text_get_size(Text *self);
void Fabric_Text_set_size(Text *self, uint32_t size);
text_t Fabric_Text_get_value(Text *self);
void Fabric_Text_set_value(Text *self, text_t value);

/**
 * ClassIndex methods
 */
classid_t Fabric_ClassIndex_get_class_id(ClassIndex *self, text_t name, error_t *status);

/**
 * DynamicList methods
 */
DynamicList *Fabric_DynamicList_allocate();
void *Fabric_DynamicList_deallocate(DynamicList *list);
error_t Fabric_DynamicList_init_with_capacity(DynamicList *self, int capacity);
error_t Fabric_DynamicList_init(DynamicList *self);
void Fabric_DynamicList_deinit(DynamicList *self);
void Fabric_DynamicList_destroy(DynamicList *self);
DynamicList *Fabric_DynamicList_new(error_t *status);
void Fabric_DynamicList_free_items(DynamicList *self, size_t member_size);
void Fabric_DynamicList_free_items_and_deinit(DynamicList *self, size_t member_size);
error_t Fabric_DynamicList_change_capacity(DynamicList *self, int new_capacity);
void* Fabric_DynamicList_at(DynamicList *self, int pos);
error_t Fabric_DynamicList_append(DynamicList *self, void* item);
error_t Fabric_DynamicList_remove_last(DynamicList *self);
int Fabric_DynamicList_count(DynamicList *self);
int Fabric_DynamicList_capacity(DynamicList *self);

/**
 * IdSet methods
 */
IdSet *Fabric_IdSet_new_with_capacity(int capacity, error_t *status);
IdSet *Fabric_IdSet_new(error_t *status);
void Fabric_IdSet_destroy(IdSet *self);
int Fabric_IdSet_get_count(IdSet *self);
bool_t Fabric_IdSet_is_empty(IdSet *self);
int Fabric_IdSet_get_capacity(IdSet *self);
bool_t Fabric_IdSet_has(IdSet *self, uint32_t id);
error_t Fabric_IdSet_add(IdSet *self, uint32_t id);
void Fabric_IdSet_remove(IdSet *self, uint32_t id);
uint32_t *Fabric_IdSet_to_array(IdSet *self, error_t *status);

/**
 * EntityMap methods
 */
EntityMap *Fabric_EntityMap_new_with_capacity(int capacity, error_t *status);
EntityMap *Fabric_EntityMap_new(error_t *status);
void Fabric_EntityMap_destroy(EntityMap *self);
int Fabric_EntityMap_get_count(EntityMap *self);
int Fabric_EntityMap_get_capacity(EntityMap *self);
bool_t Fabric_EntityMap_has_key(EntityMap *self, uint32_t key);
error_t Fabric_EntityMap_set(EntityMap *self, uint32_t key, void* entity);
void *Fabric_EntityMap_get(EntityMap *self, uint32_t key);
void Fabric_EntityMap_unset(EntityMap *self, uint32_t key);

/**
 * Internal property types
 */
#ifndef _FABRIC_PROPTYPES__
#  define _FABRIC_PROPTYPES__
#  define FABRIC_PROPTYPE_NOTHING 0x00      // nothing properties are deleted
#  define FABRIC_PROPTYPE_INTEGER 0x01
#  define FABRIC_PROPTYPE_REAL 0x02
#  define FABRIC_PROPTYPE_FRACTION 0x03     // not implemented
#  define FABRIC_PROPTYPE_COMPLEX 0x04      // not implemented
#  define FABRIC_PROPTYPE_UNICHAR 0x05
#  define FABRIC_PROPTYPE_EMPTYTEXT 0x10
#  define FABRIC_PROPTYPE_TEXT1 0x11
#  define FABRIC_PROPTYPE_TEXT2 0x12
#  define FABRIC_PROPTYPE_TEXT3 0x13
#  define FABRIC_PROPTYPE_TEXT4 0x14
#  define FABRIC_PROPTYPE_TEXT5 0x15
#  define FABRIC_PROPTYPE_TEXT6 0x16
#  define FABRIC_PROPTYPE_TEXT7 0x17
#  define FABRIC_PROPTYPE_TEXT8 0x18
#  define FABRIC_PROPTYPE_LONGTEXT 0x19
#  define FABRIC_PROPTYPE_BINARY 0x18       // not implemented
#  define FABRIC_PROPTYPE_DATETIME 0x20     // a 64-bit unix timestamp
#  define FABRIC_PROPTYPE_DATE 0x21         // not implemented
#  define FABRIC_PROPTYPE_TIME 0x22         // not implemented
#  define FABRIC_PROPTYPE_FALSE 0x30
#  define FABRIC_PROPTYPE_TRUE 0x31
#  define FABRIC_PROPTYPE_ARRAY 0x40        // not implemented
#  define FABRIC_PROPTYPE_MAP 0x41          // not implemented
#endif

/**
 * Error codes used throughout the library
 */
#ifndef _FABRIC_ERROR_CODES__
#  define _FABRIC_ERROR_CODES__
/* Generic error codes */
#  define FABRIC_OK 0x00000000
#  define FABRIC_ERROR 0x00000001
#  define FABRIC_OUT_OF_MEMORY 0x00000002
/* Error codes for the class store */
#  define FABRIC_CLASSSTORE_ERROR 0x00000100
#  define FABRIC_CLASSSTORE_INVALID_ID 0x00000101
#  define FABRIC_CLASS_DOESNT_EXIST 0x00000102
#  define FABRIC_DUPLICATE_CLASSNAME 0x00000103
#  define FABRIC_CANT_DELETE_CLASS_HAS_CHILDREN 0x00000104
#  define FABRIC_CANT_DELETE_CLASS_HAS_MEMBERS 0x00000105
#  define FABRIC_CLASSSTORE_NEEDS_RESIZE 0x00000110
/* Error codes for the label store */
#  define FABRIC_LABELSTORE_ERROR 0x00000200
#  define FABRIC_LABELSTORE_NEEDS_RESIZE 0x00000210
/* Error codes for the vertex store */
#  define FABRIC_VERTEXSTORE_ERROR 0x00000300
/* Error codes for the edge store */
#  define FABRIC_EDGESTORE_ERROR 0x00000400
/* Error codes for the property store */
#  define FABRIC_PROPERTYSTORE_ERROR 0x00000500
/* Error codes for the text store */
#  define FABRIC_TEXTSTORE_ERROR 0x00000600
/* Error codes for the index store */
#  define FABRIC_INDEXSTORE_ERROR 0x00000700
/* Error codes for graph objects */
#  define FABRIC_GRAPH_ERROR 0x00001000
/* Error codes for class objects */
#  define FABRIC_CLASS_ERROR 0x00001100
#  define FABRIC_CLASS_INVALID_ID 0x00001101
/* Error codes for label objects */
#  define FABRIC_LABEL_ERROR 0x00001200
#  define FABRIC_LABEL_INVALID_ID 0x00001201
/* Error codes for vertex objects */
#  define FABRIC_VERTEX_ERROR 0x00001300
#  define FABRIC_VERTEX_INVALID_ID 0x00001301
/* Error codes for edge objects */
#  define FABRIC_EDGE_ERROR 0x00001400
#  define FABRIC_EDGE_INVALID_ID 0x00001401
/* Error codes for property objects */
#  define FABRIC_PROPERTY_ERROR 0x00001500
#  define FABRIC_PROPERTY_INVALID_ID 0x00001501
/* Error codes for text objects */
#  define FABRIC_TEXT_ERROR 0x00001600
#  define FABRIC_TEXT_INVALID_ID 0x00001601
/* Error codes for index objects */
#  define FABRIC_INDEX_ERROR 0x00001700
#  define FABRIC_INDEX_INVALID_ID 0x00001701
/* Error codes for dynamic lists */
#  define FABRIC_DYNAMIC_LIST_ERROR 0x00002100
#endif

#endif