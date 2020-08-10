/*******************************************************************************
 *
 * Copyright (c) 2020 Seller Tamás. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

#include "managed/RefCnt.h"
#include "MockAllocator.h"

#include "1test/Test.h"

TEST_GROUP(RefCnt)
{
    struct Target: pet::RefCnt<Target, Allocator>
    {
        inline Target(int x, char y) {
            MOCK(Target)::CALL(Ctor).withParam(x).withParam(y);
        }

        inline void f()
        {
            MOCK(Target)::CALL(f);
        }

        inline virtual ~Target() {
            MOCK(Target)::CALL(Dtor);
        }
    };

    struct SubTarget: Target
    {
    	SubTarget::Ptr<SubTarget> strg;
        inline SubTarget(int x, char y): Target(x, y) {}
        inline virtual ~SubTarget() = default;
    };

    //								counter + vptr
    static_assert(sizeof(Target) == sizeof(void*) + sizeof(void*));
    static_assert(sizeof(Target::Ptr<Target>) == sizeof(void*));
};

TEST(RefCnt, Sanity)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
    {
        Target::Ptr<Target> ptr;

        CHECK(ptr == nullptr);

        ptr = Target::make(1, 'a');

        CHECK(Allocator::count == 1);

        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::count == 0);
}

TEST(RefCnt, CopyEq)
{
    {
        Target::Ptr<Target> q;

        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        {
            auto ptr = Target::make(1, 'a');

            CHECK(Allocator::count == 1);

            q = ptr;

            CHECK(Allocator::count == 1);

            MOCK(Target)::EXPECT(f);
            ptr->f();
        }

        CHECK(Allocator::count == 1);

        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::count == 0);
}

TEST(RefCnt, MoveEq)
{
    {
        Target::Ptr<Target> q;

        MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
        {
            auto ptr = Target::make(1, 'a');

            CHECK(Allocator::count == 1);

            q = pet::move(ptr);

            CHECK(Allocator::count == 1);
        }

        CHECK(Allocator::count == 1);

        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::count == 0);
}

TEST(RefCnt, CopyCtor)
{
    struct {
        inline void f(Target::Ptr<Target> q)
        {
            q->f();
        }
    } s;

    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
    {
        auto ptr = Target::make(1, 'a');

        CHECK(Allocator::count == 1);

        MOCK(Target)::EXPECT(f);
        s.f(ptr);

        CHECK(Allocator::count == 1);
        MOCK(Target)::EXPECT(Dtor);
    }

    CHECK(Allocator::count == 0);
}

TEST(RefCnt, CopyEmpty)
{
    Target::Ptr<Target> p;
    auto q = p;
}

TEST(RefCnt, MoveCtor)
{
    struct {
        inline void f(Target::Ptr<Target> &&q)
        {
            Target::Ptr<Target> r(pet::move(q));

            q->f();
            CHECK(Allocator::count == 1);
            MOCK(Target)::EXPECT(Dtor);
        }
    } s;

    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
    auto ptr = Target::make(1, 'a');

    CHECK(Allocator::count == 1);

    MOCK(Target)::EXPECT(f);
    s.f(pet::move(ptr));

    CHECK(Allocator::count == 0);
}

TEST(RefCnt, Access)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');

    Target::Ptr<Target> empty;

    auto ptr = Target::make(1, 'a');

    MOCK(Target)::EXPECT(f);
    ptr->f();

    MOCK(Target)::EXPECT(f);
    (*ptr).f();

    MOCK(Target)::EXPECT(f);
    ptr.get()->f();

    MOCK(Target)::EXPECT(Dtor);
}

TEST(RefCnt, Reset)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');

    auto ptr = Target::make(1, 'a');
    auto copy = ptr;

    CHECK(ptr);
    ptr.reset();
    CHECK(!ptr);

    CHECK(copy);

    MOCK(Target)::EXPECT(Dtor);
    copy.reset();

    CHECK(!copy);
}

TEST(RefCnt, DeleteWithNull)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');

    auto ptr = Target::make(1, 'a');

    CHECK(ptr);
    MOCK(Target)::EXPECT(Dtor);
    ptr = nullptr;
    CHECK(!ptr);
}

TEST(RefCnt, Compare)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');

    auto ptr = Target::make(1, 'a');
    Target::Ptr<Target> empty;

    CHECK(ptr);
    CHECK(!empty);

    CHECK(ptr != empty);
    CHECK(ptr == ptr);
    CHECK(empty == empty);
    CHECK(ptr != nullptr);
    CHECK(!(ptr == nullptr));
    CHECK(empty == nullptr);
    CHECK(!(empty != nullptr));

    MOCK(Target)::EXPECT(Dtor);
}

TEST(RefCnt, Subclass)
{
    MOCK(Target)::EXPECT(Ctor).withParam(1).withParam('a');
    MOCK(Target)::EXPECT(Ctor).withParam(2).withParam('b');

    SubTarget::Ptr<SubTarget> *strga;

    {
        auto a = SubTarget::make<SubTarget>(1, 'a');
        auto b = SubTarget::make<SubTarget>(2, 'b');

        a->strg = b;
        b->strg = a;

        strga = &a->strg;
    }

    MOCK(Target)::EXPECT(Dtor);
    MOCK(Target)::EXPECT(Dtor);

    *strga = nullptr;
}
