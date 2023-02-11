#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/cursor.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/stdx/string_view.hpp>

class Model {
private:
    std::map<std::string, bsoncxx::types::b_value> fields;

public:
    void addField(const std::string& name, bsoncxx::types::b_value value) {
        fields[name] = value;
    }

    void save(mongocxx::collection coll) const {
        bsoncxx::builder::stream::document doc;
      
        for (auto const& field : fields) {
            doc << field.first << field.second;
        }
      
        coll.insert_one(doc.view());
    }

    void delete_one(mongocxx::collection coll) const {
        bsoncxx::builder::stream::document doc;
      
        for (auto const& field : fields) {
            doc << field.first << field.second;
        }
      
        coll.delete_one(doc.view());
    }

    void update(mongocxx::collection coll, bsoncxx::document::view_or_value filter) const {
        bsoncxx::builder::stream::document doc;
      
        for (auto const& field : fields) {
            doc << "$set" << bsoncxx::builder::stream::open_document
                << field.first << field.second << bsoncxx::builder::stream::close_document;
        }
      
        coll.update_one(filter, doc.view());
    }

    static Model findOne(mongocxx::collection coll, bsoncxx::document::view_or_value filter) {
        Model obj;
        mongocxx::cursor cursor = coll.find_one(filter);
      
        if (cursor) {
            bsoncxx::document::view view = *cursor;
          
            for (auto const& element : view) {
                obj.addField(element.key().to_string(), element.get_value());
            }
        }
      
        return obj;
    }

    static std::vector<Model> find(mongocxx::collection coll, bsoncxx::document::view_or_value filter) {
        std::vector<Model> results;
        mongocxx::cursor cursor = coll.find(filter);
      
        for (auto const& view : cursor) {
            Model obj;
          
            for (auto const& element : view) {
                obj.addField(element.key().to_string(), element.get_value());
            }
          
            results.push_back(obj);
        }
      
        return results;
    }
};
