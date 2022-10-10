/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/*
 * This file is part of the libepubgen project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef INCLUDED_EPUBLISTSTYLEMANAGER_H
#define INCLUDED_EPUBLISTSTYLEMANAGER_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#include <boost/functional/hash.hpp>

#include <librevenge/librevenge.h>

#include "EPUBCSSProperties.h"
#include "EPUBParagraphStyleManager.h"

namespace libepubgen
{

class EPUBCSSContent;

/** Small class to manage the list style */
class EPUBListStyleManager : public EPUBParagraphStyleManager
{
public:
  struct List
  {
    typedef std::unordered_map<EPUBCSSProperties, std::string, boost::hash<EPUBCSSProperties>> ContentNameMap_t;

    //! constructor
    List() : m_contentsList(), m_level(0)
    {
    }
    //! destructor
    ~List()
    {
    }
    //! set the property correspond to a level
    void setLevel(int lvl, librevenge::RVNGPropertyList const &property, bool ordered);
    //! open a new level
    void openLevel() const
    {
      m_level++;
    }
    //! open a new level
    void closeLevel() const
    {
      if (m_level <= 0)
        return;
      m_level--;
    }

    //! return the properties of the current level
    const EPUBCSSProperties &getCurrentProperties() const;

  protected:
    //! the properties
    std::vector<EPUBCSSProperties> m_contentsList;
    //! the actual list level
    mutable int m_level;
  };
  //! constructor
  EPUBListStyleManager() : EPUBParagraphStyleManager(), m_levelNameMap(),
    m_idListMap(), m_actualIdStack()
  {
  }
  //! destructor
  ~EPUBListStyleManager() override
  {
  }
  //! add a level to the corresponding list
  void defineLevel(librevenge::RVNGPropertyList const &property, bool ordered);
  //! returns the class name corresponding to a propertylist
  std::string openLevel(librevenge::RVNGPropertyList const &pList, bool ordered);
  //! close a level
  void closeLevel();
  //! returns the classname corresponding to a list element
  std::string getClass(librevenge::RVNGPropertyList const &pList);

  //! send the data to the sink
  void send(EPUBCSSContent &out);
protected:
  //! a map content -> list level name
  List::ContentNameMap_t m_levelNameMap;
  //! a map listId -> list
  std::map<int, List> m_idListMap;
  //! the actual list id
  std::vector<int> m_actualIdStack;
private:
  EPUBListStyleManager(EPUBListStyleManager const &orig);
  EPUBListStyleManager operator=(EPUBListStyleManager const &orig);
};

}

#endif

/* vim:set shiftwidth=2 softtabstop=2 expandtab: */
