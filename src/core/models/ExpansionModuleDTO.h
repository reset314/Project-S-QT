#pragma once
#include <QJsonObject>
#include <QJsonArray>
#include <string>
#include <vector>
#include <optional>
#include "../utils/JsonHelper.h"

struct PanelConfig {
    std::string type;
    QJsonObject actions;

    static PanelConfig fromJson(const QJsonObject &obj) {
        PanelConfig d;
        d.type = JsonHelper::getString(obj, "type");
        auto a = obj.value("actions");
        d.actions = a.isObject() ? a.toObject() : QJsonObject{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["type"] = QString::fromStdString(type);
        o["actions"] = actions;
        return o;
    }
};

struct AppBarTabConfig {
    std::string title;
    std::string icon = "extension";
    int priority = 100;
    std::string route;
    PanelConfig panel;

    static AppBarTabConfig fromJson(const QJsonObject &obj) {
        AppBarTabConfig d;
        d.title = JsonHelper::getString(obj, "title");
        d.icon = JsonHelper::getString(obj, "icon", "extension");
        auto p = obj.value("priority");
        if (p.isDouble()) d.priority = p.toInt();
        d.route = JsonHelper::getString(obj, "route");
        auto pc = obj.value("panel");
        d.panel = pc.isObject() ? PanelConfig::fromJson(pc.toObject()) : PanelConfig{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["title"] = QString::fromStdString(title);
        o["icon"] = QString::fromStdString(icon);
        o["priority"] = priority;
        o["route"] = QString::fromStdString(route);
        o["panel"] = panel.toJson();
        return o;
    }
};

struct ExpansionFunction {
    std::string name;
    std::string module;
    bool returnsToAi = false;
    bool returnsToUser = false;
    std::string returnsToUserMsgType = "text";

    static ExpansionFunction fromJson(const QJsonObject &obj) {
        ExpansionFunction d;
        d.name = JsonHelper::getString(obj, "name");
        d.module = JsonHelper::getString(obj, "module");
        d.returnsToAi = JsonHelper::getBool(obj, "returns_to_ai", false);
        d.returnsToUser = JsonHelper::getBool(obj, "returns_to_user", false);
        d.returnsToUserMsgType = JsonHelper::getString(obj, "returns_to_user_msg_type", "text");
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["name"] = QString::fromStdString(name);
        o["module"] = QString::fromStdString(module);
        o["returns_to_ai"] = returnsToAi;
        o["returns_to_user"] = returnsToUser;
        o["returns_to_user_msg_type"] = QString::fromStdString(returnsToUserMsgType);
        return o;
    }
};

struct ExpansionModuleDTO {
    std::string name;
    std::string description;
    std::string category;
    std::vector<std::string> targetClients;
    std::vector<std::string> functions;
    QJsonObject ui;

    static ExpansionModuleDTO fromJson(const QJsonObject &obj) {
        ExpansionModuleDTO d;
        d.name = JsonHelper::getString(obj, "name");
        d.description = JsonHelper::getString(obj, "description");
        d.category = JsonHelper::getString(obj, "category");
        auto tc = obj.value("target_clients");
        if (tc.isArray()) {
            for (const auto &item : tc.toArray()) {
                if (item.isString()) d.targetClients.push_back(item.toString().toStdString());
            }
        }
        auto fn = obj.value("functions");
        if (fn.isArray()) {
            for (const auto &item : fn.toArray()) {
                if (item.isString()) d.functions.push_back(item.toString().toStdString());
            }
        }
        auto u = obj.value("ui");
        d.ui = u.isObject() ? u.toObject() : QJsonObject{};
        return d;
    }

    QJsonObject toJson() const {
        QJsonObject o;
        o["name"] = QString::fromStdString(name);
        o["description"] = QString::fromStdString(description);
        o["category"] = QString::fromStdString(category);
        QJsonArray tcArr;
        for (const auto &t : targetClients) tcArr.append(QString::fromStdString(t));
        o["target_clients"] = tcArr;
        QJsonArray fnArr;
        for (const auto &f : functions) fnArr.append(QString::fromStdString(f));
        o["functions"] = fnArr;
        if (!ui.isEmpty()) o["ui"] = ui;
        return o;
    }
};
