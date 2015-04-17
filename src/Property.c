/**
 * This file is part of the FabricDB library
 *
 * Author: Mark Wardle <mark@themarkside.com>
 * Created: March 24, 2015
 * Updated: March 27, 2015
 */

#ifndef _FABRIC_PROPERTY_C__
#define _FABRIC_PROPERTY_C__

#include <string.h>
#include "Internal.h"

/**
 * A property is a piece of data associated with a vertex or edge entity.
 *
 * Every property has a label which is unique among its owner's properties.
 * As such, an entity's properties can be looked up in a manner that is
 * similar to a dictionary adt.
 *+
 * An entity's properties are stored internally as a linked list.  A property's
 * next_property_id points to its owner's next property.
 *
 * A property also has a type that specifies the type of data the property
 * contains.  A properties type may be any of the following:
 *
 * 0x00 nothing (used internally)
 * 0x01 integer (64 bit, signed)
 * 0x02 real (64 bit IEEE floating point number)
 * 0x03 fraction (32 bit integral numerator and 32 bit integral denominator, not implemented)
 * 0x10 64-bit unicode character
 * 0x11 short text (string whose length is <= 8 bytes)
 * 0x12 text (string whose length > 8 bytes)
 * 0x18 binary (binary data, not implemented)
 * 0x20 datetime (unix)
 * 0x21 date
 * 0x22 time
 * 0x30 boolean_false
 * 0x31 boolean_true
 * 0x40 array (untyped, not implemented)
 * 0x41 map (embedded key-value data, not implemented)
 *
 * Null attributes are not stored.  They are marked as null by their absence.
 *
 * An entity's attributes are implemented as a linked link, hence the
 * next_property_id field which points to the next property in the list.
 *
 * The database stores property objects in 17 bytes
 *
 * +----+----+----+----+----+----+----+----+------+
 * | label_id          | next_property_id  | type |
 * +----+----+----+----+----+----+----+----+------+
 * | value                                 |
 * +----+----+----+----+----+----+----+----+
 */
typedef struct Property {
    propertyid_t id;                // The internal id of this property
    labelid_t label_id;             // The id of the label for this property
    propertyid_t next_property_id;  // The id of the next property for the property's owner
    uint8_t type;                  // A tag to identify the type of the property
    uint8_t data[8];               // 8 bytes worth of data for the property
} Property;

/**
 * Gets a property's internal id
 */
propertyid_t Fabric_Property_get_id(Property *self) {
    return self->id;
}

/**
 * Sets a property's internal id
 */
void Fabric_Property_set_id(Property *self, propertyid_t id) {
    self->id = id;
}

/**
 * Initializes a property object from its database data
 *
 * The property's id should be set before it is initialized
 *
 * Args:
 *      self: The property being initialized
 *      data: An array of bytes that represent this property in the database (17 bytes)
 *
 * Returns: FABRIC_OK on success, other error code on failure
 */
error_t Fabric_Property_init(Property *self, uint8_t *data) {
    // A Propery's id must be set externally
    if (self->id < 1) {
        return FABRIC_PROPERTY_INVALID_ID;
    }
    self->label_id = betoh32(*(labelid_t*)(data));
    self->next_property_id = betoh32(*(propertyid_t*)(data + 4));
    self->type = data[8];
    memcpy(&self->data, (data + 9), 8);

    return FABRIC_OK;
}

/**
 * Gets the id of a property's label
 */
labelid_t Fabric_Property_get_label_id(Property *self) {
    return self->label_id;
}

/**
 * Gets the label for a property
 *
 * Args:
 *      self: The property object whose label is being retrieved
 *      graph: The graph object the property belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *      The property's label
 *      status set to FABRIC_OK on success, other error number on failure
 */
Label *Fabric_Property_get_label(Property *self, Graph *graph, error_t *status) {
    LabelStore *ls = Fabric_Graph_get_label_store(graph);
    return Fabric_LabelStore_get_label(ls, self->label_id, status);
}

/**
 * Returns the id of the property's owner's next property
 */
propertyid_t Fabric_Property_get_next_property_id(Property *self) {
    return self->next_property_id;
}

/**
 * Returns the property's owner's next property
 *
 * Fabric_Propertyu_has_next_property(1) should be called on the property
 * prior to calling this method to ensure the property has a next property
 * to return.  Otherwise, an error may be returned.
 *
 * Args:
 *      self: The property object whose owner's next property is being retrieved
 *      graph: The graph object the property belongs to
 *      out: A pointer to where the result will be stored
 *
 * Returns:
 *      The property's owner's next property
 *      FABRIC_OK on success, other error number on failure
 */
Property *Fabric_Property_get_next_property(Property *self, Graph *graph, error_t *status) {
    PropertyStore *ps = Fabric_Graph_get_property_store(graph);
    return Fabric_PropertyStore_get_property(ps, self->next_property_id, status);
}

/**
 * Returns whether or not a property has a next property
 */
bool_t Fabric_Property_has_next_property(Property *self) {
    return self->next_property_id != 0;
}

/**
 * Gets the internal property type of a property
 *
 * For a detailed list of possible return values, see the
 * section on fabric property types in the Internal.h
 * header file.
 */
uint8_t Fabric_Property_get_type(Property *self) {
    return self->type;
}

/**
 * Sets the internal property type of a property
 *
 * For a detailed list of possible types, see the
 * section on fabric property types in the Internal.h
 * header file.
 */
void Fabric_Property_set_type(Property *self, uint8_t type) {
    self->type = type;
}

/**
 * Gets the raw data for the property
 */
uint8_t* Fabric_Property_get_data(Property *self) {
    return self->data;
}

/**
 * Returns the property's data as a 64 bit integer
 */
int64_t Fabric_Property_get_integer_value(Property *self) {
    return betoh64(*((int64_t*)self->data));
}

/**
 * Sets the value of the property's data as an integer
 */
void Fabric_Property_set_integer_value(Property *self, int64_t value) {
    *((int64_t*)self->data) = htobe64(value);
}

/**
 * Return's the property's data as a 64-bit floating point number
 */
float64_t Fabric_Property_get_real_value(Property *self) {
    uint64_t tmp = betoh64(*((uint64_t*)self->data));
    return *((float64_t*)&tmp);
}

/**
 * Sets the value of the property's data as a float
 */
void Fabric_Property_set_real_value(Property *self, float64_t value) {
    *((uint64_t*)self->data) = htobe64(*((uint64_t*)(&value)));
}

/**
 * Returns 1 if the property is of boolean type, 0 if not
 */
bool_t Fabric_Property_is_boolean(Property *self) {
    return self->type == FABRIC_PROPTYPE_FALSE ||
            self->type == FABRIC_PROPTYPE_TRUE;
}

/**
 * Return's the property's data as a boolean
 *
 * Note: does not verify the validity of the value
 * Note: the value of a boolean property is set by
 *       its type.
 */
bool_t Fabric_Property_get_boolean_value(Property *self) {
    if (self->type == FABRIC_PROPTYPE_TRUE) {
        return TRUE;
    }
    // type should be FABRIC_PROPTYPE_FALSE,
    // but we ain't gonna check.  That's other folk's job.
    return FALSE;
}

/**
 * Returns 1 if the property holds text, 0 otherwise
 */
bool_t Fabric_Property_is_text(Property *self) {
    return self->type >= FABRIC_PROPTYPE_EMPTYTEXT &&
            self->type <= FABRIC_PROPTYPE_LONGTEXT;
}

/**
 * Returns 1 if the property holds text and is not a long text,
 * 0 otherwise
 */
bool_t Fabric_Property_is_short_text(Property *self) {
    return self->type >= FABRIC_PROPTYPE_EMPTYTEXT &&
            self->type < FABRIC_PROPTYPE_LONGTEXT;
}

/**
 * Returns the short text length in bytes of a property
 */
int Fabric_Property_get_short_text_length(Property *self) {
    return self->type - FABRIC_PROPTYPE_EMPTYTEXT;
}

/**
 * Stores a property's short text in dest
 *
 * Args:
 *      self: The property whose text is being retrieved
 *            Should be a short text property
 *      dest: Pointer to where the text will be stored.
 *            Should have a minimum size of the length
 *            of the text stored in the property plus 1,
 *            since this function will append a null
 *            terminating character
 */
void Fabric_Property_get_short_text(Property *self, text_t dest) {
    int len = Fabric_Property_get_short_text_length(self);
    Fabric_strncpy(dest, self->data, len);
    dest[len] = '\0';
}

/**
 * Sets the property's short text value
 *
 * Important: The size based property type should be set for the property
 * before this function is called since it uses it determine how much
 * text to copy from the source.  As such, source should have a length
 * at least equal to
 */
void Fabric_Property_set_short_text(Property *self, text_t source) {
    int len = Fabric_Property_get_short_text_length(self);
    Fabric_strncpy(self->data, source, len);
}

/**
 * Gets the id of a property's text object
 *
 * This should only be called on a property with a type
 * of FABRIC_PROPTYPE_LONGTEXT. Short text is handled
 * differently.  Long text is any text with size
 * greater than 8 bytes (not including the null terminator).
 */
textid_t Fabric_Property_get_text_value_id(Property *self) {
    return (textid_t) betoh64(*((uint64_t*)self->data));
}

/**
 * Sets the id of a property's text object
 *
 * This should only be called on a propert with a type
 * of FABRIC_PROPTYPE_LONGTEXT. Short text is handled
 * differently.  Short text is any text with size
 * less than or equal to 8 bytes.
 */
void Fabric_Property_set_text_value_id(Property *self, textid_t text_id) {
    *((uint64_t*)self->data) = htobe64((uint64_t)text_id);
}

#endif