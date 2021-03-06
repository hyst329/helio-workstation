/*
    This file is part of Helio Workstation.

    Helio is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Helio is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Helio. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

class Serializable;

class Config final : private Timer
{
public:

    explicit Config(int timeoutToSaveMs = 10000);
    ~Config() override;

    static String getDeviceId();

    static bool contains(const Identifier &key);
    static void set(const Identifier &key, const var &value, bool delayedSave = true);
    static String get(const Identifier &key, const String &fallback = {});

    static void save(const Serializable *serializer, const Identifier &key);
    static void save(const Serializable &serializer, const Identifier &key);
    static void load(Serializable *serializer, const Identifier &key);
    static void load(Serializable &serializer, const Identifier &key);

private:

    void onConfigChanged();
    bool saveIfNeeded();
    bool reload();

    void saveConfigFor(const Identifier &key, const Serializable *serializer);
    void loadConfigFor(const Identifier &key, Serializable *serializer);

    void setProperty(const Identifier &key, const var &value, bool delayedSave);
    String getProperty(const Identifier &key, const String &fallback) const noexcept;

    bool containsPropertyOrChild(const Identifier &key) const noexcept;

private:

    void timerCallback() override;

    InterProcessLock fileLock;
    File propertiesFile;
    
    FlatHashMap<Identifier, var, IdentifierHash> properties;
    FlatHashMap<Identifier, ValueTree, IdentifierHash> children;

    // As the app development moves forward, some properties
    // become deprecated, but they will still present in config file,
    // so we need to track the unused ones and never save them:
    mutable FlatHashSet<Identifier, IdentifierHash> usedKeys;

    bool needsSaving;
    int saveTimeout;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Config)
};
