# Copyright (C) 2016, Sam Thursfield (sam@afuera.me.uk)
# Copyright (C) 2019, Sam Thursfield (sam@afuera.me.uk)
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.

"""
Tests failure cases of tracker-extract.
"""

import os
import shutil
import unittest as ut

import minertest


VALID_FILE = os.path.join(
    os.path.dirname(__file__), 'test-extraction-data', 'audio',
    'mp3-id3v2.4-1.mp3')
VALID_FILE_CLASS = 'http://www.tracker-project.org/temp/nmm#MusicPiece'
VALID_FILE_TITLE = 'Simply Juvenile'

TRACKER_EXTRACT_FAILURE_DATA_SOURCE = 'tracker:extractor-failure-data-source'


class ExtractorDecoratorTest(minertest.CommonTrackerMinerTest):
    def test_reextraction(self):
        """Tests whether known files are still re-extracted on user request."""
        miner_fs = self.miner_fs
        store = self.tracker

        # Insert a valid file and wait extraction of its metadata.
        file_path = os.path.join(self.indexed_dir, os.path.basename(VALID_FILE))
        shutil.copy(VALID_FILE, file_path)
        try:
            file_id, file_urn = store.await_resource_inserted(
                VALID_FILE_CLASS, title=VALID_FILE_TITLE)

            # Remove a key piece of metadata.
            #   (Writeback must be disabled in the config so that the file
            #   itself is not changed).
            store.update(
                'DELETE { GRAPH ?g { <%s> nie:title ?title } }'
                ' WHERE { GRAPH ?g { <%s> nie:title ?title } }' % (file_urn, file_urn))
            store.await_property_changed(VALID_FILE_CLASS, file_id, 'nie:title')
            assert not store.ask('ASK { <%s> nie:title ?title }' % file_urn)

            # Request re-indexing (same as `tracker index --file ...`)
            miner_fs.index_file('file://' + os.path.join(self.indexed_dir, file_path))

            # The extractor should reindex the file and re-add the metadata that we
            # deleted, so we should see the nie:title property change.
            store.await_property_changed(VALID_FILE_CLASS, file_id, 'nie:title')

            title_result = store.query('SELECT ?title { <%s> nie:title ?title }' % file_urn)
            assert len(title_result) == 1
            self.assertEqual(title_result[0][0], VALID_FILE_TITLE)
        finally:
            os.remove(file_path)


if __name__ == '__main__':
    ut.main(verbosity=2)
