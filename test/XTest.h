//-----------------------------------------------------------------------------
// Copyright (C) 2007 Matt Lawrence
//
// File: XTest.h
//
// Description:
//
// XTest unit testing framework.
//----------------------------------------------------------------------------

#ifndef __XTEST_H__
#define __XTEST_H__

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define XTS_ASSERT_EQUALS(_expected, _actual) _XTS_ASSERT_COMPARE(==, _expected, _actual)
#define XTS_ASSERT_DIFFERS(_expected, _actual) _XTS_ASSERT_COMPARE(!=, _expected, _actual)

#define XTS_ASSERT_EQUALS_CSTR(_expected, _actual) _XTS_ASSERT_COMPARE_CSTR(==, _expected, _actual)
#define XTS_ASSERT_DIFFERS_CSTR(_expected, _actual) _XTS_ASSERT_COMPARE_CSTR(!=, _expected, _actual)

#define XTS_ASSERT_THROWS_EQUALS(_stmt, _excType, _excExpected) XTS_ASSERT_THROWS_GRANULAR(_stmt, _excType e, _excExpected, e)
#define XTS_ASSERT_THROWS_GRANULAR(_stmt, _exc, _excExpectedValue, _excActualValue) { \
   bool _didThrow = false; \
   try {\
      _stmt; \
   } catch(_exc) { \
      if((_excExpectedValue) != (_excActualValue)) { \
         exceptionComparisonTestFailed(__FILE__, __LINE__, #_stmt, #_excExpectedValue, #_excActualValue, toString(_excExpectedValue), toString(_excActualValue)); \
         return; \
      } \
      _didThrow = true; \
   } catch(...) { \
      exceptionTypeTestFailed(__FILE__, __LINE__, #_stmt, #_exc); \
      return; \
   } \
   if(!_didThrow) { \
      exceptionDidntThrow(__FILE__, __LINE__, #_stmt); \
      return; \
   } \
}

#define XTS_ASSERT_THROWS_TYPE(_stmt, _excType) { \
   bool _didThrow = false; \
   try { \
      _stmt; \
   } catch(const _excType&) { \
      _didThrow = true; \
   } catch(...) { \
      exceptionTypeTestFailed(__FILE__, __LINE__, #_stmt, #_excType); \
      return; \
   } \
   if(!_didThrow) { \
      exceptionDidntThrow(__FILE__, __LINE__, #_stmt); \
      return; \
   } \
}

#define XTS_ASSERT_THROWS_ANYTHING(_stmt) { \
   bool _didThrow = false; \
   try { \
      _stmt; \
   } catch(...) { \
      _didThrow = true; \
   } \
   if(!_didThrow) { \
      exceptionDidntThrow(__FILE__, __LINE__, #_stmt); \
      return; \
   } \
}

#define XTS_ASSERT_NOTHROW(_stmt) { \
   try { \
      _stmt; \
   } catch(...) { \
      exceptionShouldntThrow(__FILE__, __LINE__, #_stmt); \
      return; \
   } \
}

#define _XTS_ASSERT_COMPARE(_operator, _expected, _actual) { \
   if(!((_expected) _operator (_actual))) { \
      comparisonTestFailed(__FILE__, __LINE__, toString(_expected), toString(_actual), #_expected, #_actual, #_operator); \
      return; \
   } \
} 

#define _XTS_ASSERT_COMPARE_CSTR(_operator, _expected, _actual) { \
   if(!(strcmp((_expected), (_actual)) _operator 0)) { \
      comparisonTestFailed(__FILE__, __LINE__, toString(_expected), toString(_actual), #_expected, #_actual, #_operator); \
      return; \
   } \
}

#define XTS_TEST(_collection, _name) \
class _XTS##_collection##_name##Test : public XTest { \
public: \
   _XTS##_collection##_name##Test() : XTest(#_collection, #_name) {} \
   ~_XTS##_collection##_name##Test() {} \
   void runTest(); \
} _XTS##_collection##_name##TestInstance; \
void _XTS##_collection##_name##Test::runTest()

class XTestResult {
public:
   XTestResult();
   ~XTestResult();

   void setFailed(bool);
   void setFile(const std::string&);
   void setLine(unsigned int);
   void setDescription(const std::string&);

   bool failed() const;
   const std::string& file() const;
   unsigned int line() const;
   const std::string& description() const;

private:
   bool mFailed;
   std::string mFile;
   unsigned int mLine;
   std::string mDescription;

   XTestResult(const XTestResult&);
   XTestResult& operator=(const XTestResult&);
};

//-----------------------------------------------------------------------------
class XTest {
public:
   XTest(const std::string& Collection, const std::string& Name);
   virtual ~XTest();

   virtual void runTest() = 0;

   const std::string& name() const;

   XTestResult& result();
   const XTestResult& result() const;

protected:
   void comparisonTestFailed(
      const std::string& File,
      unsigned int Line,
      const std::string& Expected,
      const std::string& Actual,
      const std::string& ExpectedLiteral,
      const std::string& ExpectedActual,
      const std::string& Operator
   );

   void exceptionTypeTestFailed(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement,
      const std::string& ExpectedType
   );

   void exceptionDidntThrow(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement
   );

   void exceptionShouldntThrow(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement
   );
   
   void exceptionComparisonTestFailed(
      const std::string& File,
      unsigned int Line,
      const std::string& Statement,
      const std::string& Expected,
      const std::string& Actual,
      const std::string& ExpectedLiteral,
      const std::string& ExpectedActual
   );

   template<class T>
   static std::string toString(const T& Value) {
      std::ostringstream Stream;
      Stream << Value;
      return Stream.str();
   }

private:
   std::string mName;
   XTestResult mResult;

   XTest(const XTest&);
   XTest& operator=(const XTest&);
};

class XTestSuite;

//-----------------------------------------------------------------------------
class XTestCollection {
public:
   XTestCollection(const std::string& Name, XTestSuite& Parent);
   ~XTestCollection();

   void addTest(XTest&);

   const std::string& name() const;

   unsigned int countOfTest() const;
   XTest& test(unsigned int);
   const XTest& test(unsigned int) const;

private:
   std::vector<XTest*> mTests;
   std::string mName;
   XTestSuite* mpParent;

   XTestCollection(const XTestCollection&);
   XTestCollection& operator=(const XTestCollection&);
};

//-----------------------------------------------------------------------------
class XTestFormatter;

class XTestSuite {
public:
   XTestSuite(const std::string&);
   ~XTestSuite();

   static XTestSuite& instance();
   void addTest(const std::string& Collection, XTest& Test);

   void runAllTests();

   unsigned int countOfCollection() const;
   XTestCollection& collection(unsigned int);
   const XTestCollection& collection(unsigned int) const;

   const std::string& name() const;

private:   
   std::vector<XTestCollection*> mCollections;
   std::string mName;
   XTestFormatter* mpFormatter;

   XTestSuite(const XTestSuite&);
   XTestSuite& operator=(const XTestSuite&);
};

//-----------------------------------------------------------------------------
class XTestFormatter {
public:
   XTestFormatter(std::ostream& Stream);
   virtual ~XTestFormatter();

   virtual void enterSuite(const XTestSuite&) = 0;
   virtual void exitSuite(const XTestSuite&) = 0;

   virtual void enterCollection(const XTestCollection&) = 0;
   virtual void exitCollection(const XTestCollection&) = 0;

   virtual void enterTest(const XTest&) = 0;
   virtual void exitTest(const XTest&) = 0;

protected:
   std::ostream& stream();

private:
   std::ostream* _mpStream;
};

//-----------------------------------------------------------------------------
class XTestEmptyFormatter : public XTestFormatter {
public:
   XTestEmptyFormatter() : XTestFormatter(std::cout) {}
   ~XTestEmptyFormatter() {}

   void enterSuite(const XTestSuite&) {}
   void exitSuite(const XTestSuite&) {}

   void enterCollection(const XTestCollection&) {}
   void exitCollection(const XTestCollection&) {}

   void enterTest(const XTest&) {}
   void exitTest(const XTest&) {}
};

//-----------------------------------------------------------------------------
class XTestMinimalFormatter : public XTestFormatter {
public:
   XTestMinimalFormatter(std::ostream&);
   ~XTestMinimalFormatter();

   void enterSuite(const XTestSuite&);
   void exitSuite(const XTestSuite&);

   void enterCollection(const XTestCollection&);
   void exitCollection(const XTestCollection&);

   void enterTest(const XTest&);
   void exitTest(const XTest&);
};

#endif
